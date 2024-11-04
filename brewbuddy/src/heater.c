// Heater Library (Definition)
// File: heater.c
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

// Hardware configuration:
// GPIO PIN PB2 - Heater Relay

//-----------------------------------------------------------------------------
// Device Includes
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "heater.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

#define PORTA_DATA      0x400043FC
#define PORTB_DATA      0x400053FC
#define PORTC_DATA      0x400063FC
#define PORTD_DATA      0x400073FC
#define PORTE_DATA      0x400243FC
#define PORTF_DATA      0x400253FC

#define HEATER                (*((volatile uint32_t *)(0x42000000 + (PORTB_DATA-0x40000000)*32 + 2*4)))
#define RED_LED               (*((volatile uint32_t *)(0x42000000 + (PORTF_DATA-0x40000000)*32 + 1*4)))
#define BLUE_LED              (*((volatile uint32_t *)(0x42000000 + (PORTF_DATA-0x40000000)*32 + 2*4)))

#define HEATER_MASK           (1 << 2)
#define RED_LED_MASK          (1 << 1)
#define BLUE_LED_MASK         (1 << 2)

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize Heater
void heater_init(void)
{
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1; // Enable Port B clock
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5; // Enable Port F clock
    __asm("   NOP"); // Wait cycle 1
    __asm("   NOP"); // Wait cycle 2
    __asm("   NOP"); // Wait cycle 3
    GPIO_PORTB_DIR_R |= HEATER_MASK; // Configure Pin Direction Output
    GPIO_PORTF_DIR_R |= RED_LED_MASK;
    GPIO_PORTF_DIR_R |= BLUE_LED_MASK;

    GPIO_PORTB_DEN_R |= HEATER_MASK; // Enable Digital Pins
    GPIO_PORTF_DEN_R |= RED_LED_MASK;
    GPIO_PORTF_DEN_R |= BLUE_LED_MASK;
}

// Turn Heater On
void heater_on(void)
{
    // Turn ON Heater and ON Red LED (Heater ON Indicator)
    RED_LED = 1;
    BLUE_LED = 0;
    HEATER = 1;
}

// Turn Heater Off
void heater_off(void)
{
    // Turn OFF Heater and ON Blue LED (Heater OFF Indicator)
    RED_LED = 0;
    BLUE_LED = 1;
    HEATER = 0;
}
