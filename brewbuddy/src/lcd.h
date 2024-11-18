// LCD Library (Header)
// File: lcd.h
// Group Members:
// - Andrew Howard
// - James Revette
// - Madison Gage

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL with I2C0 20x4 LCD with PCF8574
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:
// SCL - PA6
// SDA - PA7
// HD44780-based 16x2, 20x2, 16x4 20x4 LCD display
// Display driven by PCF8574 I2C 8-bit I/O expander at address 0x27
// I2C devices on I2C bus 0 with 2kohm pullups on SDA and SCL
// Display RS, R/W, E, backlight enable, and D4-7 connected to PCF8574 P0-7

//-----------------------------------------------------------------------------
// Preprocessor Directives
//-----------------------------------------------------------------------------

#ifndef LCD_H_
#define LCD_H_

//-----------------------------------------------------------------------------
// Device Includes
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void lcd_init();
void lcd_writeData(uint8_t add, uint8_t data);
void lcd_writeCommand(uint8_t command);
void lcd_writeText(char c);
void lcd_puts(uint8_t row, uint8_t col, const char str[]);
void lcd_itoa(char *str, uint32_t num);
void lcd_print(uint32_t ti, uint32_t tm, uint32_t tp, bool heater_status);

#endif
