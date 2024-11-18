// LCD Library (Definition)
// File: lcd.c
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
// Device Includes
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "lcd.h"
#include "wait.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

#define LCD_ADD 0x27
#define LCD_RS 1
#define LCD_RW 2
#define LCD_E  4
#define LCD_BACKLIGHT 8

#define I2C1SCL               (1 << 6)
#define I2C1SDA               (1 << 7)

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize Hardware (I2C1)
void lcd_init()
{
    // Enable clocks
    SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R1;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
    _delay_cycles(3);
    

    // Configure GPIO for SCL
    GPIO_PORTA_DEN_R |= I2C1SCL;
    GPIO_PORTA_AFSEL_R |= I2C1SCL;
    GPIO_PORTA_ODR_R &= ~I2C1SCL;
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xF0FFFFFF) | GPIO_PCTL_PA6_I2C1SCL;

    // Configure GPIO for SDA
    GPIO_PORTA_DEN_R |= I2C1SDA;
    GPIO_PORTA_AFSEL_R |= I2C1SDA;
    GPIO_PORTA_ODR_R |= I2C1SDA;
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0x0FFFFFFF) | GPIO_PCTL_PA7_I2C1SDA;

    // Configure I2C0 peripheral
    I2C1_MCR_R = 0;                                     // disable to program
    I2C1_MTPR_R = 19;                                   // (40MHz/2) / (6+4) / (19+1) = 100kbps
    I2C1_MCR_R = I2C_MCR_MFE;                           // master
    I2C1_MCS_R = I2C_MCS_STOP;

    // Wait for device to come out of reset
    waitMicrosecond(20000); // Wait at least 15 ms for LCD to power up

    // Send initialization commands
    // Use 3 consecutive 0x30 commands to ensure the LCD enters 8-bit mode first
    lcd_writeData(LCD_ADD, 0x30 | LCD_E);
    lcd_writeData(LCD_ADD, 0x30);
    waitMicrosecond(5000); // Wait 5 ms

    lcd_writeData(LCD_ADD, 0x30 | LCD_E);
    lcd_writeData(LCD_ADD, 0x30);
    waitMicrosecond(200); // Wait 200 us

    lcd_writeData(LCD_ADD, 0x30 | LCD_E);
    lcd_writeData(LCD_ADD, 0x30);

    // Set to 4-bit mode
    lcd_writeData(LCD_ADD, 0x20 | LCD_E);
    lcd_writeData(LCD_ADD, 0x20);

    // Configure LCD in 4-bit mode
    lcd_writeCommand(0x28); // 4-bit interface, 2 lines, 5x8 font
    lcd_writeCommand(0x0C); // Display on, no cursor, no blink
    lcd_writeCommand(0x06); // Increment cursor after each character
    lcd_writeCommand(0x01); // Clear display
    waitMicrosecond(2000);     // Wait for display to clear
}

// Write Data to LCD
void lcd_writeData(uint8_t add, uint8_t data)
{
    // send start cond, send address, get ack,
    // send data, get ack, send stop condition
    I2C1_MSA_R = add << 1 | 0; // add:r/~w=0
    I2C1_MDR_R = data;
    I2C1_MICR_R = I2C_MICR_IC;
    I2C1_MCS_R = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP;
    waitMicrosecond(50);                           // Nonblocking
    // while ((I2C1_MRIS_R & I2C_MRIS_RIS) == 0);  // Blocking
}

// Write Command to LCD
void lcd_writeCommand(uint8_t command)
{
    // output MSN
    lcd_writeData(LCD_ADD, (command & 0xF0) | LCD_E | LCD_BACKLIGHT);
    lcd_writeData(LCD_ADD, (command & 0xF0) | LCD_BACKLIGHT);
    // output LSN
    lcd_writeData(LCD_ADD, (command << 4) | LCD_E | LCD_BACKLIGHT);
    lcd_writeData(LCD_ADD, (command << 4) | LCD_BACKLIGHT);
}

// Write Text to LCD
void lcd_writeText(char c)
{
    // output MSN
    lcd_writeData(LCD_ADD, (c & 0xF0) | LCD_E | LCD_RS | LCD_BACKLIGHT);
    lcd_writeData(LCD_ADD, (c & 0xF0) | LCD_RS | LCD_BACKLIGHT);
    // output LSN
    lcd_writeData(LCD_ADD, (c << 4) | LCD_E | LCD_RS | LCD_BACKLIGHT);
    lcd_writeData(LCD_ADD, (c << 4) | LCD_RS | LCD_BACKLIGHT);
}

// Write String to LCD
void lcd_puts(uint8_t row, uint8_t col, const char str[])
{
    uint8_t i = 0;
    lcd_writeCommand(0x80 + (row & 1) * 64 + (row & 2) * 10 + col);
    while (str[i] != NULL)
        lcd_writeText(str[i++]);
}

void lcd_itoa(char *str, uint32_t num)
{
    char tmp[10];
    uint8_t i = 0;
    uint8_t j = 0;

    // check for zero
    if (num == 0)
    {
        str[0] = '0';
        str[1] = '\0';
    }

    else
    {
        while (num != 0)
        {
            tmp[i++] = (num % 10) + '0';
            num = num / 10;
        }

        str[i] = '\0';

        // reverse the string and print
        do
        {
            str[j++] = tmp[--i];
        }
        while (i != 0);
    }
    return;
}

// Print to LCD
void lcd_print(uint32_t ti, uint32_t tm, uint32_t tp, bool heater_status)
{
    char str_tp[10];
    char str_tm[10];
    char str_ti[10];

    lcd_itoa(str_tp, tp);
    lcd_itoa(str_tm, tm);
    lcd_itoa(str_ti, ti);

    lcd_puts(0, 0, "Infrared:       ");
    lcd_puts(0, 12, str_ti);
    lcd_puts(0, 16, "F");
    lcd_puts(1, 0, "Thermistor:     ");
    lcd_puts(1, 12, str_tm);
    lcd_puts(1, 16, "F");
    lcd_puts(2, 0, "User Input:     ");
    lcd_puts(2, 12, str_tp);
    lcd_puts(2, 16, "F");
    lcd_puts(3, 0, "Heater:         ");
    heater_status ? lcd_puts(3, 12, "ON") : lcd_puts(3, 12, "OFF");
}
