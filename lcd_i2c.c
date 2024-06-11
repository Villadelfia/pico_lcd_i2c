#include <stdlib.h>
#include "pico/time.h"
#include "lcd_i2c.h"

#define LCD_CLEAR_DISPLAY 0x01
#define LCD_RETURN_HOME 0x02
#define LCD_ENTRY_MODE_SET 0x04
#define LCD_DISPLAY_CONTROL 0x08
#define LCD_CURSOR_SHIFT 0x10
#define LCD_FUNCTION_SET 0x20
#define LCD_SET_CG_RAM_ADDR 0x40
#define LCD_SET_DD_RAM_ADDR 0x80
#define LCD_ENTRY_RIGHT 0x00
#define LCD_ENTRY_LEFT 0x02
#define LCD_ENTRY_SHIFT_INCREMENT 0x01
#define LCD_ENTRY_SHIFT_DECREMENT 0x00
#define LCD_DISPLAY_ON 0x04
#define LCD_DISPLAY_OFF 0x00
#define LCD_CURSOR_ON 0x02
#define LCD_CURSOR_OFF 0x00
#define LCD_BLINK_ON 0x01
#define LCD_BLINK_OFF 0x00
#define LCD_DISPLAY_MOVE 0x08
#define LCD_CURSOR_MOVE 0x00
#define LCD_MOVE_RIGHT 0x04
#define LCD_MOVE_LEFT 0x00
#define LCD_8_BIT_MODE 0x10
#define LCD_4_BIT_MODE 0x00
#define LCD_2_LINE 0x08
#define LCD_1_LINE 0x00
#define LCD_10_HEIGHT 0x04
#define LCD_8_HEIGHT 0x00
#define LCD_BACKLIGHT_ON 0x08
#define LCD_BACKLIGHT_OFF 0x00

#define EN_B 0b00000100
#define RW_B 0b00000010
#define RS_B 0b00000001

struct lcd_i2c {
    i2c_inst_t *i2c;
    uint8_t addr;
    uint8_t displayfunction;
    uint8_t displaycontrol;
    uint8_t displaymode;
    uint8_t cols;
    uint8_t rows;
    uint8_t charheight;
    uint8_t backlightval;
    uint8_t x;
    uint8_t y;
};

void p_display_init(lcd_i2c *lcd);
int p_write_raw(lcd_i2c *lcd, uint8_t data);
void p_write_4b(lcd_i2c *lcd, uint8_t data);
void p_send(lcd_i2c *lcd, uint8_t data, uint8_t mode);
void p_command(lcd_i2c *lcd, uint8_t data);
void p_write(lcd_i2c *lcd, uint8_t data);
void p_advance_position(lcd_i2c *lcd);


lcd_i2c *lcd_setup(i2c_inst_t *i2c, uint baudrate, uint sda, uint scl) {
    lcd_i2c *lcd = malloc(sizeof(lcd_i2c));
    if(lcd == NULL) return NULL;
    lcd->i2c = i2c;
    i2c_init(i2c, baudrate);
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_pull_up(scl);
    return lcd;
}

lcd_i2c *lcd_setup_default() {
    lcd_i2c *lcd = malloc(sizeof(lcd_i2c));
    if(lcd == NULL) return NULL;
    lcd->i2c = i2c0;
    i2c_init(i2c0, 400000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);
    return lcd;
}

void lcd_begin(lcd_i2c *lcd, uint8_t addr, uint8_t cols, uint8_t rows, uint8_t charheight) {
    lcd->addr = addr;
    lcd->cols = cols;
    lcd->rows = rows;
    lcd->backlightval = LCD_BACKLIGHT_ON;
    lcd->charheight = charheight == 10 ? LCD_10_HEIGHT : LCD_8_HEIGHT;
    p_display_init(lcd);
}

void lcd_clear(lcd_i2c* lcd, bool sleep) {
    p_command(lcd, LCD_CLEAR_DISPLAY);
    if(sleep) sleep_us(1520);
}

void lcd_home(lcd_i2c* lcd, bool sleep) {
    p_command(lcd, LCD_RETURN_HOME);
    if(lcd_get_ltr(lcd)) {
        lcd->x = 0;
        lcd->y = 0;
    } else {
        lcd->x = lcd->cols-1;
        lcd->y = 0;
    }
    if(sleep) sleep_us(1520);
}

void lcd_set_cursor_location(lcd_i2c* lcd, uint8_t x, uint8_t y) {
    static uint8_t y_offsets[] = {0x00, 0x40, 0x14, 0x54};
    if(y > 3)  y = 3;
    if(x > 19) x = 19;
    p_command(lcd, LCD_SET_DD_RAM_ADDR | (x + y_offsets[y]));
    lcd->x = x;
    lcd->y = y;
}

void lcd_set_display(lcd_i2c* lcd, bool enabled) {
    if(enabled) lcd->displaycontrol |= LCD_DISPLAY_ON;
    else        lcd->displaycontrol &= ~LCD_DISPLAY_ON;
    p_command(lcd, LCD_DISPLAY_CONTROL | lcd->displaycontrol);
}

bool lcd_get_display(lcd_i2c* lcd) {
    return (lcd->displaycontrol & LCD_DISPLAY_ON) == LCD_DISPLAY_ON;
}

void lcd_set_cursor(lcd_i2c* lcd, bool enabled) {
    if(enabled) lcd->displaycontrol |= LCD_CURSOR_ON;
    else        lcd->displaycontrol &= ~LCD_CURSOR_ON;
    p_command(lcd, LCD_DISPLAY_CONTROL | lcd->displaycontrol);
}

bool lcd_get_cursor(lcd_i2c* lcd) {
    return (lcd->displaycontrol & LCD_CURSOR_ON) == LCD_CURSOR_ON;
}

void lcd_set_blink(lcd_i2c* lcd, bool enabled) {
    if(enabled) lcd->displaycontrol |= LCD_BLINK_ON;
    else        lcd->displaycontrol &= ~LCD_BLINK_ON;
    p_command(lcd, LCD_DISPLAY_CONTROL | lcd->displaycontrol);
}

bool lcd_get_blink(lcd_i2c* lcd) {
    return (lcd->displaycontrol & LCD_BLINK_ON) == LCD_BLINK_ON;
}

void lcd_set_ltr(lcd_i2c* lcd) {
    lcd->displaymode |= LCD_ENTRY_LEFT;
    p_command(lcd, LCD_ENTRY_MODE_SET | lcd->displaymode);
}

bool lcd_get_ltr(lcd_i2c* lcd) {
    return (lcd->displaymode & LCD_ENTRY_LEFT) == LCD_ENTRY_LEFT;
}

void lcd_set_rtl(lcd_i2c* lcd) {
    lcd->displaymode &= ~LCD_ENTRY_LEFT;
    p_command(lcd, LCD_ENTRY_MODE_SET | lcd->displaymode);
}

bool lcd_get_rtl(lcd_i2c* lcd) {
    return (lcd->displaymode & LCD_ENTRY_LEFT) != LCD_ENTRY_LEFT;
}

void lcd_set_autoscroll(lcd_i2c* lcd, bool enabled) {
    if(enabled) lcd->displaymode |= LCD_ENTRY_SHIFT_INCREMENT;
    else        lcd->displaymode &= ~LCD_ENTRY_SHIFT_INCREMENT;
    p_command(lcd, LCD_ENTRY_MODE_SET | lcd->displaymode);
}

bool lcd_get_autoscroll(lcd_i2c* lcd) {
    return (lcd->displaymode & LCD_ENTRY_SHIFT_INCREMENT) == LCD_ENTRY_SHIFT_INCREMENT;
}

void lcd_set_backlight(lcd_i2c* lcd, bool enabled) {
    lcd->backlightval = enabled ? LCD_BACKLIGHT_ON : LCD_BACKLIGHT_OFF;
    p_write_raw(lcd, 0);
}

bool lcd_get_backlight(lcd_i2c* lcd) {
    return lcd->backlightval == LCD_BACKLIGHT_ON;
}

void lcd_scroll_x(lcd_i2c* lcd, int8_t amount) {
    while(amount < 0) {
        amount++;
        p_command(lcd, LCD_CURSOR_SHIFT | LCD_DISPLAY_MOVE | LCD_MOVE_LEFT);
    }

    while(amount > 0) {
        amount--;
        p_command(lcd, LCD_CURSOR_SHIFT | LCD_DISPLAY_MOVE | LCD_MOVE_RIGHT);
    }
}

void lcd_create_character(lcd_i2c* lcd, uint8_t location, uint8_t data[]) {
    if(location > 7) return;
    p_command(lcd, LCD_SET_CG_RAM_ADDR | (location << 3));
    for(uint8_t i = 0; i < (lcd->charheight == LCD_10_HEIGHT ? 10 : 8); i++) p_write(lcd, data[i]);
    lcd_set_cursor_location(lcd, lcd->x, lcd->y);
}

void lcd_create_all_characters(lcd_i2c* lcd, uint8_t characters, uint8_t data[]) {
    p_command(lcd, LCD_SET_CG_RAM_ADDR);
    for(uint8_t i = 0; i < ((lcd->charheight == LCD_10_HEIGHT ? 10 : 8)*characters); i++) p_write(lcd, data[i]);
    lcd_set_cursor_location(lcd, lcd->x, lcd->y);
}

void lcd_print(lcd_i2c* lcd, const char* str) {
    for(size_t i = 0; str[i] != '\0'; i++) {
        p_write(lcd, str[i]);
        p_advance_position(lcd);
    }
}

void lcd_print_s(lcd_i2c* lcd, const char* str, size_t length) {
    for(size_t i = 0; i < length; i++) {
        p_write(lcd, str[i]);
        p_advance_position(lcd);
    }
}

void lcd_print_c(lcd_i2c* lcd, char c) {
    p_write(lcd, c);
    p_advance_position(lcd);
}

void lcd_show_buffer(lcd_i2c* lcd, const char* buffer) {
    if(!lcd_get_ltr(lcd)) lcd_set_ltr(lcd);
    if(lcd_get_autoscroll(lcd)) lcd_set_autoscroll(lcd, false);
    if(lcd_get_cursor(lcd)) lcd_set_cursor(lcd, false);
    if(lcd_get_blink(lcd)) lcd_set_blink(lcd, false);
    size_t i = 0;
    for(size_t y = 0; y < lcd->rows; y++) {
        lcd_set_cursor_location(lcd, 0, y);
        for(size_t x = 0; x < lcd->cols; x++) {
            p_write(lcd, buffer[i++]);
        }
    }
    lcd->x = 0;
    lcd->y = 0;
}

void p_display_init(lcd_i2c *lcd) {    
    // Check how long we've been on.
    uint32_t ms_since_boot = to_ms_since_boot(get_absolute_time());

    // The LCD requires power to be applied for at least 40ms.
    if (ms_since_boot < 40) {
        ms_since_boot = 40 - ms_since_boot;
        sleep_ms(ms_since_boot);
    }

    // Put the LCD into 4 bit mode.
    p_write_raw(lcd , 0x00);
    sleep_ms(10);
    p_write_4b(lcd, 0x03 << 4);
    sleep_us(4100);
    p_write_4b(lcd, 0x03 << 4);
    sleep_us(4100);
    p_write_4b(lcd, 0x03 << 4);
    sleep_us(100);
    p_write_4b(lcd, 0x02 << 4);

    // Configure display function.
    lcd->displayfunction = LCD_4_BIT_MODE | lcd->charheight;
    if(lcd->rows > 1) lcd->displayfunction |= LCD_2_LINE;
    p_command(lcd, LCD_FUNCTION_SET | lcd->displayfunction);

    // Configure the display control.
    lcd->displaycontrol = LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF;
    p_command(lcd, LCD_DISPLAY_CONTROL | lcd->displaycontrol);

    // Configure the display mode.
    lcd->displaymode = LCD_ENTRY_LEFT | LCD_ENTRY_SHIFT_DECREMENT;
    p_command(lcd, LCD_ENTRY_MODE_SET | lcd->displaymode);

    // Clear the display.
    lcd_clear(lcd, true);
    lcd_home(lcd, true);
}

int p_write_raw(lcd_i2c *lcd, uint8_t data) {
    data |= lcd->backlightval;
    return i2c_write_blocking(lcd->i2c, lcd->addr, &data, 1, false);
}

void p_write_4b(lcd_i2c *lcd, uint8_t data) {
    // Write the data.
    p_write_raw(lcd, data);

    // Pulse enable.
    p_write_raw(lcd, data | EN_B);
    sleep_us(1);
    p_write_raw(lcd, data & ~EN_B);
    sleep_us(37);
}

void p_send(lcd_i2c *lcd, uint8_t data, uint8_t mode) {
    uint8_t h = data & 0xF0;
    uint8_t l = (data << 4) & 0xF0;
    p_write_4b(lcd, h | mode);
    p_write_4b(lcd, l | mode);
}

void p_command(lcd_i2c *lcd, uint8_t data) {
    p_send(lcd, data, 0);
}

void p_write(lcd_i2c *lcd, uint8_t data) {
    p_send(lcd, data, RS_B);
}

void p_advance_position(lcd_i2c* lcd) {
    if(lcd_get_ltr(lcd)) {
        lcd->x++;
        lcd->x %= lcd->cols;
        if(lcd->x == 0) {
            lcd->y++;
            lcd->y %= lcd->rows;
            if(lcd->rows > 2) lcd_set_cursor_location(lcd, lcd->x, lcd->y);
        }
    } else {
        lcd->x--;
        if(lcd->x == 255) {
            lcd->x = lcd->cols-1;
            lcd->y++;
            lcd->y %= lcd->rows;
            if(lcd->rows > 2) lcd_set_cursor_location(lcd, lcd->x, lcd->y);
        }
    }
}
