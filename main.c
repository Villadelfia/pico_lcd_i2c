#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "lcd_i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

uint8_t chars[] = {
    0b10101,
    0b01010,
    0b10101,
    0b01010,
    0b10101,
    0b01010,
    0b10101,
    0b01010,

    0b01010,
    0b10101,
    0b01010,
    0b10101,
    0b01010,
    0b10101,
    0b01010,
    0b10101,

    0b01010,
    0b01010,
    0b01010,
    0b01010,
    0b01010,
    0b01010,
    0b01010,
    0b01010,

    0b10101,
    0b10101,
    0b10101,
    0b10101,
    0b10101,
    0b10101,
    0b10101,
    0b10101,

    0b11111,
    0b00000,
    0b11111,
    0b00000,
    0b11111,
    0b00000,
    0b11111,
    0b00000,

    0b00000,
    0b11111,
    0b00000,
    0b11111,
    0b00000,
    0b11111,
    0b00000,
    0b11111,

    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b11111,
    0b11111,
    0b11111,
    0b11111,

    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b00000,
    0b00000,
    0b00000,
    0b00000
};

int main() {
    stdio_init_all();

    // Setting up a display.
    lcd_i2c* lcd = lcd_setup_default();
    lcd_begin(lcd, 0x27, 20, 4, 8);

    // Creating characters.
    for(uint8_t i = 0; i < 8; i++) lcd_create_character(lcd, i, chars + (i*8));

    // Displaying stuff.
    char buffer[20*4] = {0};
    int i = 0;
    char c = 0;
    while (true) {
        lcd_show_buffer(lcd, buffer);
        buffer[i++] = c++;
        i %= 20*4;
        sleep_ms(5);
    }
}
