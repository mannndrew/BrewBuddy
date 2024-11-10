// UART Interface Library (Definition)
// File: interface.c
// Group Members:
// - Andrew Howard
// - James Revette
// - Madison Gage

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:
// UART Interface:
//   U0TX (PA1) and U0RX (PA0) are connected to the 2nd controller
//   The USB on the 2nd controller enumerates to an ICDI interface and a virtual COM port

//-----------------------------------------------------------------------------
// Device Includes
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "interface.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

// PortA masks
#define UART_TX_MASK 2
#define UART_RX_MASK 1

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

uint32_t inf_getUINT(USER_DATA *data, uint8_t argc)
{
    int32_t result = 0;
    int32_t digit = 0;
    int8_t i = 0;
    char *str, c;

    if ((argc < data->fieldCount) && data->fieldType[argc] == 'n')
    {
        str = &(data->buffer)[data->fieldPosition[argc]];
        c = str[i];

        while (str[i] != '\0' && '0' <= str[i] && str[i] <= '9')
        {
            digit = c - '0';
            result = result * 10 + digit;
            c = str[++i];
        }
    }
    return result;
}

void inf_printUINT(uint32_t num)
{
    char str[10];
    uint8_t i = 0;

    // check for zero
    if (num == 0)
    {
        inf_putc('0');
    }

    else
    {
        while (num != 0)
        {
            str[i++] = num % 10 + '0';
            num = num / 10;
        }

        // reverse the string and print
        do
        {
            inf_putc(str[--i]);
        }
        while (i != 0);
    }
    return;
}

void inf_printHeader(uint32_t tm, uint32_t tp)
{
    char degree_symbol[4] = {0xC2, 0xB0, 'F', '\0'};
    inf_puts("\x1B[?25l"); // Hide Cursor
    inf_puts("\x1B[s"); // Save Cursor Position
    inf_puts("\x1B[1;1H"); // Move Cursor to Line 1
    inf_puts("\x1B[K"); // Clear Line
    inf_puts("Thermistor Temperature: ");
    inf_printUINT(tm);
    inf_puts(degree_symbol);
    inf_puts("\x1B[2;1H"); // Move Cursor to Line 2
    inf_puts("\x1B[K"); // Clear Line
    inf_puts("User Input Temperature: ");
    inf_printUINT(tp);
    inf_puts(degree_symbol);
    inf_puts("\x1B[u"); // Restore Cursor Position
    inf_puts("\x1B[?25h"); // Show Cursor
}

void inf_clearScreen(uint32_t start, uint32_t end)
{
    uint32_t i;
    inf_puts("\x1B[s"); // Save Cursor Position
    for (i = start; i <= end; i++)
    {
        inf_puts("\x1B[");  // Move Cursor to Line i
        inf_printUINT(i);
        inf_puts(";1H");  
        inf_puts("\x1B[K"); // Clear Line
    }
    inf_puts("\x1B[u"); // Restore Cursor Position
}

void inf_setCursor(uint32_t line)
{
    inf_puts("\x1B[");  // Move Cursor to Line i
    inf_printUINT(line);
    inf_puts(";1H");  
}

void inf_cursorOn()
{
    inf_puts("\x1B[?25h"); // Show Cursor
}

void inf_cursorOff()
{
    inf_puts("\x1B[?25l"); // Hide Cursor
}

void inf_boldOn()
{
    inf_puts("\x1B[1m"); // Bold On
}

void inf_boldOff()
{
    inf_puts("\x1B[22m"); // Bold Off
}

bool inf_isNumber(char *str, uint8_t *len)
{
    uint8_t i = 0;  // initialize index to 0
    bool match = (str[i] != '\0' && str[i] != ' '); // initialize match to true if null or space

    while (str[i] != '\0' && str[i] != ' ') // while valid
    {
        if (!('0' <= str[i] && str[i] <= '9')) // if not digit
        {
            match = false;
        }
        i++;           // increment index
    }

    *len = i;          // set length to index
    return match;   // return true if length is not 0
}

bool inf_isAlpha(char *str, uint8_t *len)
{
    uint8_t i = 0;  // initialize index to 0
    bool match = (str[i] != '\0' && str[i] != ' '); // initialize match to true if null or space

    while (str[i] != '\0' && str[i] != ' ') // while valid
    {
        if (!(('A' <= str[i] && str[i] <= 'Z') || ('a' <= str[i] && str[i] <= 'z'))) // if not alphabetic
        {
            match = false;
        }
        i++;           // increment index
    }

    *len = i;          // set length to index
    return match;   // return true if length is not 0
}

bool inf_isCombo(char *str, uint8_t *len)
{
    uint8_t i = 0;  // initialize index to 0
    bool match = (str[i] != '\0' && str[i] != ' '); // initialize match to true if null or space

    while (str[i] != '\0' && str[i] != ' ') // while valid
    {
        if (!(('A' <= str[i] && str[i] <= 'Z') || ('a' <= str[i] && str[i] <= 'z') || ('0' <= str[i] && str[i] <= '9'))) // if not alphanumeric
        {
            match = false;
        }
        i++;           // increment index
    }

    *len = i;          // set length to index
    return match;   // return true if length is not 0
}

bool inf_isCommand(USER_DATA *data, const char cmd[], uint8_t argc)
{
    uint8_t i;
    bool match = true;

    if (data->fieldCount != argc + 1)
        match = false;

    for (i = 0; !(data->buffer[i] == '\0' || cmd[i] == '\0'); i++)
    {
        if (data->buffer[i] != cmd[i])
            match = false;
    }

    if (data->buffer[i] != cmd[i])
        match = false;

    return match;
}

bool inf_strCompare(char str1[], char str2[])
{
    uint8_t i;
    bool match = true;

    for (i = 0; !(str1[i] == '\0' || str2[i] == '\0'); i++)
    {
        if (str1[i] != str2[i])
            match = false;
    }

    if (str1[i] != str2[i])
        match = false;

    return match;
}

// Initialize UART
void inf_init()
{
    // Enable clocks
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
    __asm("   NOP"); // Wait cycle 1
    __asm("   NOP"); // Wait cycle 2
    __asm("   NOP"); // Wait cycle 3

    // Configure UART0 GPIO pins
    GPIO_PORTA_DR2R_R |= UART_TX_MASK;                            // set drive strength to 2mA (not needed since default configuration -- for clarity)
    GPIO_PORTA_DEN_R |= UART_TX_MASK | UART_RX_MASK;              // enable digital on UART0 pins
    GPIO_PORTA_AFSEL_R |= UART_TX_MASK | UART_RX_MASK;            // use peripheral to drive PA0, PA1
    GPIO_PORTA_PCTL_R &= ~(GPIO_PCTL_PA1_M | GPIO_PCTL_PA0_M);    // clear bits 0-7
    GPIO_PORTA_PCTL_R |= GPIO_PCTL_PA1_U0TX | GPIO_PCTL_PA0_U0RX; // select UART0 to drive pins PA0 and PA1: default, added for clarity


    // Configure UART0 to 115200
    UART0_CTL_R = 0;                                              // turn-off UART0 to allow safe programming
    UART0_CC_R = UART_CC_CS_SYSCLK;                               // use system clock (40 MHz)
    UART0_IBRD_R = 21;                                            // r = 40 MHz / (Nx19.2kHz), set floor(r)=130, where N=16
    UART0_FBRD_R = 45;                                            // round(fract(r)*64)=13
    UART0_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_FEN;              // configure for 8N1 w/ 16-level FIFO
    UART0_CTL_R = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN;  // enable TX, RX, and module
}

// Blocking function that writes a serial character when the UART buffer is not full
void inf_putc(char c)
{
    while (UART0_FR_R & UART_FR_TXFF);  // wait if uart0 tx fifo full
    UART0_DR_R = c;                     // write character to fifo
}

// Blocking function that writes a string when the UART buffer is not full
void inf_puts(char* str)
{
    uint8_t i = 0;                      // initialize index to string start
    while (str[i] != '\0')              // loop until null-terminator
        inf_putc(str[i++]);             // send character and increment index
}

// Blocking function that returns with serial char once the buffer is not empty
char inf_getc()
{
    while (UART0_FR_R & UART_FR_RXFE);  // wait if uart0 rx fifo empty
    return UART0_DR_R & 0xFF;           // get character from fifo
}

// Blocking function that returns with serial string once the buffer is not empty
void inf_getCommand(USER_DATA *data)
{
    uint8_t count = 0;  // initialize string index to 0
    char c = 0;         // initialize character to 0

    while (c != 13 && count != MAX_CHARS) // while not carriage return and not max chars
    {
        c = inf_getc(); // get character
        inf_putc(c);    // echo character

        if ((c == 8 || c == 127) && 0 < count) // if (backspace or delete) and not start
            count--;                           // decrement string index

        else if (32 <= c)                      // if printable character
            (data->buffer)[count++] = c;       // store character and increment string index
    }
    (data->buffer)[count] = '\0';              // null-terminate string (after max or carriage return)
    inf_puts("\r\n");                          // print newline and carriage return
    return;
}

void inf_parseCommand(USER_DATA *data)
{
    char *str = data->buffer;  // initialize string to the buffer
    uint8_t idx = 0;           // initialize string (entire) index to 0
    uint8_t len = 0;           // initialize string (field) length to 0
    data->fieldCount = 0;      // initialize argument count to 0

    while (str[idx] != '\0' && data->fieldCount != MAX_FIELDS)
    {
        len = 1; // initialize field length to 1

        if (str[idx] == ' ')
        {
            str[idx] = '\0';                                // Null-terminate string
            idx++;                                          // Increment string index
        }

        else if (inf_isNumber(&str[idx], &len))
        {
            (data->fieldType)[data->fieldCount] = 'n';      // Set field type to number
            (data->fieldPosition)[data->fieldCount] = idx;  // Set field position from i
            (data->fieldCount)++;                           // Increment field count
            idx += len;                                     // Increment string index by field length
        }

        else if (inf_isAlpha(&str[idx], &len))
        {
            (data->fieldType)[data->fieldCount] = 'a';      // Set field type to alphabetic
            (data->fieldPosition)[data->fieldCount] = idx;  // Set field position from i
            (data->fieldCount)++;                           // Increment field count
            idx += len;                                     // Increment string index by field length
        }

        else
        {
            (data->fieldType)[data->fieldCount] = 'c';      // Set field type to alphanumeric
            (data->fieldPosition)[data->fieldCount] = idx;  // Set field position from i
            (data->fieldCount)++;                           // Increment field count
            idx += len;                                     // Increment string index by field length
        }
    }
    return;
}

void inf_doCommand(USER_DATA *data, uint32_t *tp, uint32_t tm)
{
    char degree_symbol[4] = {0xC2, 0xB0, 'F', '\0'};
    if (inf_isCommand(data, "clear", 0))
    {
        inf_clearScreen(4, 10);
    }

    else if (inf_isCommand(data, "get", 1))
    {
        if(inf_strCompare(&data->buffer[data->fieldPosition[1]], "-tp"))
        {
            inf_puts("User Input Temperature: ");
            inf_printUINT(*tp);
            inf_puts(degree_symbol);
            inf_puts("\r\n\n");
        }
            
        else if(inf_strCompare(&data->buffer[data->fieldPosition[1]], "-tm"))
        {
            inf_puts("Thermistor Temperature: ");
            inf_printUINT(tm);
            inf_puts(degree_symbol);
            inf_puts("\r\n\n");
        }

        else
            inf_puts("Invalid command...type 'help' for a list of commands.\r\n\n");
    }

    else if (inf_isCommand(data, "set", 2))
    {
        if (inf_strCompare(&data->buffer[data->fieldPosition[1]], "-tp"))
        {
            *tp = inf_getUINT(data, 2);
            inf_puts("Setting the value of the user input temperature...\r\n\n");
        }

        else
            inf_puts("Invalid command...type 'help' for a list of commands.\r\n\n");
    }

    else if (inf_isCommand(data, "help", 0))
    {
        inf_puts("Available Commands:                                         \r\n");
        inf_puts("   get -tp       Get the value of the user input temperature\r\n");
        inf_puts("   get -tm       Get the value of the thermistor temperature\r\n");
        inf_puts("   set -tp       Set the value of the user input temperature\r\n");
        inf_puts("   clear         Clear the terminal screen                  \r\n\n");
    }

    else
    {
        inf_puts("Invalid command...type 'help' for a list of commands.\r\n\n");
    }

    return;
}
