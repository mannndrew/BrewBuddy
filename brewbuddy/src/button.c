// Button Library (Definition)
// File: button.c
// Group Members:
// - Andrew Howard
// - James Revette
// - Madison Gage

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL Evaluation Board
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration
// GPIO PIN PF4 - Heater Button

//-----------------------------------------------------------------------------
// Device Includes
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "button.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

#define PORTA_DATA      0x400043FC
#define PORTB_DATA      0x400053FC
#define PORTC_DATA      0x400063FC
#define PORTD_DATA      0x400073FC
#define PORTE_DATA      0x400243FC
#define PORTF_DATA      0x400253FC

#define BUTTON                (*((volatile uint32_t *)(0x42000000 + (PORTF_DATA-0x40000000)*32 + 4*4)))
#define BUTTON_MASK           (1 << 4)

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void button_init(void) // Initialize Button
{
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5; // Enable Port F clock
    __asm("   NOP"); // Wait cycle 1
    __asm("   NOP"); // Wait cycle 2
    __asm("   NOP"); // Wait cycle 3
    GPIO_PORTF_DIR_R &= ~BUTTON_MASK; // Configure Pin Direction Output
    GPIO_PORTF_DEN_R |=  BUTTON_MASK; // Enable Digital Pins
    GPIO_PORTF_PUR_R |=  BUTTON_MASK; // Enable Pull-Up Resistor
}

bool button_status(void) // Get Button Status
{
    return !BUTTON;
}
