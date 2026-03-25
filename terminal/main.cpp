#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/i2c_slave.h"
#include "hardware/irq.h"
#include <stdio.h>
#include <string.h>

#include "picovga.h"
#include "i2c_config.h"
#include "terminal.h"

// text screen (mono character, format GF_MTEXT)
ALIGNED u8 TextBuf[2*TEXTSIZE];

// copy of font
ALIGNED u8 Font_Copy[sizeof(FontBold8x8)];

bool led = true;
char data[100];
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

void backspace() {
    if (PrintX == 0) return;
    PrintX--;
    PrintChar(' ');
    PrintX--;
}

void scroll_up() {
	for (size_t r=1; r<TEXTH; r++) {
		memcpy(&PrintBuf[TEXTWB*(r-1)], &PrintBuf[TEXTWB*r], TEXTWB);
	}
    for (size_t i = 0; i < TEXTWB; i++) {
        memset(&PrintBuf[TEXTWB*(TEXTH-1)], 0, TEXTWB);
    }
    PrintX = 0; // TODO: I don't know why this is necessary, but without this the first character after the scroll does not print.
    PrintY = TEXTH - 1;
}

void printChar(uint8_t c) {
    //TODO: add autoscroll toggle
    PrintChar(data[0]);
    if (PrintY >= TEXTH) {
        scroll_up();
    }
    // TODO: 2 seems weird, but it is working correctly, why?
    if (PrintX >= TEXTW - 2) {
        PrintChar(CHAR_LF);
    }
    if (PrintY >= TEXTH) {
        scroll_up();
    }
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

                printChar(data[0]);
            } else {
                switch (data[0]) // first byte defines command
                {
                case PRINT_STRING:
                    data[data_size] = 0x0; // add null terminator.
                    PrintText(&data[1]);  // TODO: does not wrap.
                case CONTROL_COMMAND:
                    switch (data[1]) {
                        case CLS:
                            PrintClear();
                            PrintHome();
                            break;
                        case BACKSPACE:
                            backspace();
                            break;

                        default:
                            break;
                    }
                    break;
                
                default:
                    break;
                }
            }

            #ifdef DEBUG
            printf("Cursor position: PrintX, PrintY: %d,%d\n", PrintX, PrintY);
            #endif

            // een check dat data groter dan 0 is?

            new_data = false;
        }
    }
}
