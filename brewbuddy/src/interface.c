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
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "interface.h"

// PortA masks
#define UART_TX_MASK 2
#define UART_RX_MASK 1

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize UART
void inf_init()
{
    // Enable clocks
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
    __asm("   NOP"); // Wait cycle 1
    __asm("   NOP"); // Wait cycle 2
    __asm("   NOP"); // Wait cycle 3

    // Configure UART0 pins
    GPIO_PORTA_DR2R_R |= UART_TX_MASK;                  // set drive strength to 2mA (not needed since default configuration -- for clarity)
    GPIO_PORTA_DEN_R |= UART_TX_MASK | UART_RX_MASK;    // enable digital on UART0 pins
    GPIO_PORTA_AFSEL_R |= UART_TX_MASK | UART_RX_MASK;  // use peripheral to drive PA0, PA1
    GPIO_PORTA_PCTL_R &= ~(GPIO_PCTL_PA1_M | GPIO_PCTL_PA0_M); // clear bits 0-7
    GPIO_PORTA_PCTL_R |= GPIO_PCTL_PA1_U0TX | GPIO_PCTL_PA0_U0RX; // select UART0 to drive pins PA0 and PA1: default, added for clarity


    // Configure UART0 to 115200
    UART0_CTL_R = 0;                                    // turn-off UART0 to allow safe programming
    UART0_CC_R = UART_CC_CS_SYSCLK;                     // use system clock (40 MHz)
    UART0_IBRD_R = 21;                                  // r = 40 MHz / (Nx19.2kHz), set floor(r)=130, where N=16
    UART0_FBRD_R = 45;                                  // round(fract(r)*64)=13
    UART0_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_FEN;    // configure for 8N1 w/ 16-level FIFO
    UART0_CTL_R = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN; // enable TX, RX, and module
}

// Print the menu to the terminal if printTRUE is true
void init_printMenu(uint32_t printTRUE)
{
    puts("Menu:\r\n");
    return;
}

// Blocking function that writes a serial character when the UART buffer is not full
void putc(char c)
{
    while (UART0_FR_R & UART_FR_TXFF);               // wait if uart0 tx fifo full
    UART0_DR_R = c;                                  // write character to fifo
}

// Blocking function that writes a string when the UART buffer is not full
void puts(char* str)
{
    uint8_t i = 0;
    while (str[i] != '\0')
        putc(str[i++]);
}

// Blocking function that returns with serial char once the buffer is not empty
char getc()
{
    while (UART0_FR_R & UART_FR_RXFE);               // wait if uart0 rx fifo empty
    return UART0_DR_R & 0xFF;                        // get character from fifo
}

// Blocking function that returns with serial string once the buffer is not empty
void gets(USER_DATA *data)
{
    uint8_t count = 0;
    char c = 0;

    while (c != 13 && count != MAX_CHARS)
    {
        c = getc();
        putc(c);

        if ((c == 8 || c == 127) && 0 < count)
            count--;

        else if (32 <= c)
            (data->buffer)[count++] = c;
    }
    (data->buffer)[count] = '\0';

    puts("\r\n");
    return;
}
