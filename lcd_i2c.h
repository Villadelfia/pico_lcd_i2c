#ifndef LCD_I2C_H_
#define LCD_I2C_H_
#include "pico/stdlib.h"
#include "hardware/i2c.h"

typedef struct lcd_i2c lcd_i2c;

/* Sets up an lcd instance for the given i2c hardware block, baudrate, and pins.
 *
 * Returns NULL in case of failure.
 **/
lcd_i2c* lcd_setup(i2c_inst_t* i2c, uint baudrate, uint sda, uint scl);

/* Sets up an lcd instance on i2c0, 400kHz, and pins 4 and 5.
 *
 * Returns NULL in case of failure.
 **/
lcd_i2c* lcd_setup_default();

/* Configure a setup lcd_i2c instance for use. You must provice the i2c
 * address, the size of the display, and the character height.
 *
 * Common values for cols include 8, 16, 20, 40.
 * Common values for rows include 1, 2, 4.
 * Character height is usually 8, but some displays have a 10 pixel high font.
 * 
 * No error checking is performed at all.
 **/
void lcd_begin(lcd_i2c* lcd, uint8_t addr, uint8_t cols, uint8_t rows, uint8_t charheight);

/* Clears the LCD. If sleep is true, the function will wait the required 1.52ms
 * before returning.
 **/
void lcd_clear(lcd_i2c* lcd, bool sleep);

/* Returns the cursor to the top left (top right in RTL mode.) If sleep is 
 * true, the function will wait the required 1.52ms before returning.
 **/
void lcd_home(lcd_i2c* lcd, bool sleep);

/* Sets the cursor to a specific position.
 **/
void lcd_set_cursor_location(lcd_i2c* lcd, uint8_t x, uint8_t y);

/* Enables/disables the display.
 **/
void lcd_set_display(lcd_i2c* lcd, bool enabled);

/* Gets the display on/off state.
 **/
bool lcd_get_display(lcd_i2c* lcd);

/* Enables/disables cursor display.
 **/
void lcd_set_cursor(lcd_i2c* lcd, bool enabled);

/* Gets the cursor display state.
 **/
bool lcd_get_cursor(lcd_i2c* lcd);

/* Enables/disables cursor blinking.
 **/
void lcd_set_blink(lcd_i2c* lcd, bool enabled);

/* Gets the cursor blink state.
 **/
bool lcd_get_blink(lcd_i2c* lcd);

/* Sets the display to LTR.
 **/
void lcd_set_ltr(lcd_i2c* lcd);

/* Gets whether the display is set to LTR.
 **/
bool lcd_get_ltr(lcd_i2c* lcd);

/* Sets the display to RTL.
 **/
void lcd_set_rtl(lcd_i2c* lcd);

/* Gets whether the display is set to RTL.
 **/
bool lcd_get_rtl(lcd_i2c* lcd);

/* Enables/disables display auto-shift.
 **/
void lcd_set_autoscroll(lcd_i2c* lcd, bool enabled);

/* Gets display auto-shift state.
 **/
bool lcd_get_autoscroll(lcd_i2c* lcd);

/* Enables/disables the backlight.
 **/
void lcd_set_backlight(lcd_i2c* lcd, bool enabled);

/* Gets the backlight state.
 **/
bool lcd_get_backlight(lcd_i2c* lcd);

/* Scrolls the display by amount character cells.
 *
 * If amount is positive, the scroll direction will be to the right,
 * otherwise it will be to the left.
 **/
void lcd_scroll_x(lcd_i2c* lcd, int8_t amount);

/* Creates a custom character at location 0-7. Data must be an array of
 * height bytes. These can then be printed using bytes 0-7.
 **/
void lcd_create_character(lcd_i2c* lcd, uint8_t location, uint8_t data[]);

/* Creates a series of custom characters starting at location 0. Data must
 * contain characters * height bytes.
 **/
void lcd_create_all_characters(lcd_i2c* lcd, uint8_t characters, uint8_t data[]);

/* Prints a string to the screen starting from the current cursor position.
 **/
void lcd_print(lcd_i2c* lcd, const char* str);

/* Same as lcd_print, but with an explicit length parameter.
 **/
void lcd_print_s(lcd_i2c* lcd, const char* str, size_t length);

/* Same as lcd_print, but printing a single character.
 **/
void lcd_print_c(lcd_i2c* lcd, char c);

/* Copies the buffer to the screen from the top left. Will reset the screen
 * to LTR without autoscroll, blinking, or cursor. Buffer must be of size 
 * cols*rows.
 **/
void lcd_show_buffer(lcd_i2c* lcd, const char* buffer);

#endif