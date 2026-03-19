#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/i2c_slave.h"
#include "hardware/irq.h"
#include <stdio.h>
#include <string.h>

#include "picovga.h"
#include "i2c_config.h"

#define WIDTH	640	// screen width in pixels
#define HEIGHT	480	// screen height
#define FONTW	8	// font width
#define FONTH	8	// font height
#define TEXTW	(WIDTH/FONTW) // text width (=80)
#define TEXTH	(HEIGHT/FONTH) // text height (=60)
#define TEXTWB	TEXTW // text width byte (=80)
#define TEXTSIZE (TEXTWB*TEXTH) // text box size in bytes (=4800)

// text screen (mono character, format GF_MTEXT)
ALIGNED u8 TextBuf[2*TEXTSIZE];

// copy of font
ALIGNED u8 Font_Copy[sizeof(FontBold8x8)];

bool led = true;
uint8_t data[100];
bool new_data;
size_t data_size;
size_t data_size_counter;

// TODO: gracefully handle transmissions that come before the previous one is handled
static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
    led = !led;
    gpio_put(PICO_DEFAULT_LED_PIN, led);
    switch (event)  {
    case I2C_SLAVE_RECEIVE:
        data[data_size_counter++] = i2c_read_byte_raw(i2c);
        break;
    case I2C_SLAVE_FINISH:
        data_size = data_size_counter;
        new_data = true;
        data_size_counter = 0;
        break;
    default:
        break;
    }
}

void setup_slave() {
    i2c_init(i2c0, I2C_BAUDRATE);
    gpio_init(I2C_SLAVE_SDA_PIN);
    gpio_set_function(I2C_SLAVE_SDA_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SLAVE_SDA_PIN);
    gpio_init(I2C_SLAVE_SCL_PIN);
    gpio_set_function(I2C_SLAVE_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SLAVE_SCL_PIN);

    i2c_slave_init(i2c0, I2C_SLAVE_ADDRESS, &i2c_slave_handler);
    irq_set_enabled(I2C0_IRQ, true);
}

void cls() {
    PrintClear();
    PrintHome();
}

int main() {
    #ifdef DEBUG
    stdio_init_all();
    #endif

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, led);

    setup_slave();
    irq_set_enabled(I2C0_IRQ, true);

    // Initialize video mode
    memcpy(Font_Copy, FontBold8x8, sizeof(FontBold8x8));
	Video(DEV_VGA, RES_VGA, FORM_TEXT8, TextBuf, Font_Copy);
    PrintSetCol(PC_LTMAGENTA);

    #ifdef DEBUG
    PrintText("Hello World!");
    #endif

    while (true) {
        sleep_us(10);
        
        if (new_data) {
            if (data_size == 1) {
                #ifdef DEBUG
                printf("printing: %d\n", data);
                #endif

                if (PrintX == TEXTW) {
                    PrintChar(CHAR_LF);
                }
                PrintChar(data[0]);
            } else {
                switch (data[0]) // first byte defines commant
                {
                case 0x1: // replace with control command macro
                    switch (data[1]) {
                        case 0x0: // replace with CLS macro
                            PrintClear();
                            PrintHome();
                            break;

                        default:
                            break;
                    }
                    break;
                
                default:
                    break;
                }
            }

            // een check dat data groter dan 0 is?

            new_data = false;
        }
    }
}
