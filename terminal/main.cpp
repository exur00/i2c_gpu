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
uint8_t data;
bool new_data;

// TODO: implement >1 byte transmissions
static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
    if (event == I2C_SLAVE_RECEIVE) {
        led = !led;
        gpio_put(PICO_DEFAULT_LED_PIN, led);
        data = i2c_read_byte_raw(i2c);
        new_data = true;
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
    stdio_init_all();
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, led);

    setup_slave();
    irq_set_enabled(I2C0_IRQ, true);

    // Initialize video mode
    memcpy(Font_Copy, FontBold8x8, sizeof(FontBold8x8));
	Video(DEV_VGA, RES_VGA, FORM_TEXT8, TextBuf, Font_Copy);
    PrintSetCol(PC_LTMAGENTA);

    PrintText("Hello World!");

    while (true) {
        sleep_us(10);
        
        if (new_data) {
            new_data = false;
            printf("printing: %d\n", data);
            if (PrintX == TEXTW) {
                PrintChar(CHAR_LF);
            }
            PrintChar(data);
        }
    }
}
