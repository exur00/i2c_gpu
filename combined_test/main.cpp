#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/i2c_slave.h"
#include "hardware/irq.h"
#include <stdio.h>

#include "picovga.h"
#include "i2c_config.h"

// EGA (512x400) frame buffer for display
ALIGNED u8 Framebuffer[512*400];

bool led = true;
uint8_t data;
uint8_t old_data;

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
    if (event == I2C_SLAVE_RECEIVE) {
        led = !led;
        gpio_put(PICO_DEFAULT_LED_PIN, led);
        data = i2c_read_byte_raw(i2c);
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

int main() {
    stdio_init_all();
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, led);

    setup_slave();
    irq_set_enabled(I2C0_IRQ, true);

    // Initialize video mode
	Video(DEV_VGA, RES_EGA, FORM_8BIT, Framebuffer);

    // Draw text
	DrawText(&Canvas, "Hello World!", (512-12*8*4)/2, (400-8*4)/2,
		COL_WHITE, FontBoldB8x16, 16, 4, 4);

    while (true) {
        //tight_loop_contents();
        sleep_ms(1000);
        char textBuf[20];
        printf("data: %d\n", data);
        
        if (old_data != data) {
            DrawClear(&Canvas);
            sprintf(textBuf, "Writing: %c", data);
            DrawText(&Canvas, textBuf, (512-12*8*4)/2, (400-8*4)/2,
                COL_WHITE, FontBoldB8x16, 16, 4, 4);
        }
    }
}
