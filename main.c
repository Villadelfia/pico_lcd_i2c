#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "lcd_i2c.h"

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
    lcd_create_all_characters(lcd, 8, chars);

    // Displaying stuff.
    char buffer[20*4] = {0};
    char c = 0;
    while (true) {
        memset(buffer + (0*20), c++, 20);
        memset(buffer + (1*20), c++, 20);
        memset(buffer + (2*20), c++, 20);
        memset(buffer + (3*20), c++, 20);
        lcd_show_buffer(lcd, buffer);
        sleep_ms(1000);
    }
}
