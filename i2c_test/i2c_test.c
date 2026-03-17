#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/i2c_slave.h"
#include "hardware/irq.h"
#include <stdio.h>

#include "i2c_config.h"

bool led = true;
uint8_t data;

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

    while (true) {
        //tight_loop_contents();
        sleep_ms(1000);
        printf("data: %d\n", data);
    }
}
