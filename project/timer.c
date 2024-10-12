// Timer Library (Definition)
// File: timer.c

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL Evaluation Board
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

//-----------------------------------------------------------------------------
// Device Includes
//-----------------------------------------------------------------------------

#include "tm4c123gh6pm.h"
#include "timer.h"
#include <stdint.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize Wide Timer 0
void timer_init()
{
    // Enable clocks with 3 wait cycles
    SYSCTL_RCGCWTIMER_R |= SYSCTL_RCGCWTIMER_R0; // Enable Wide Timer 0 clock
    __asm("   NOP"); // Wait cycle 1
    __asm("   NOP"); // Wait cycle 2
    __asm("   NOP"); // Wait cycle 3

    // Configure Wide Timer 0 (Full Configuration)
    WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;            // Disable Wide Timer 0 A
    WTIMER0_CFG_R = TIMER_CFG_32_BIT_TIMER;      // Set Wide Timer 0 as 64-bit timer
    WTIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;     // Set Timer 0 to Periodic Mode
    WTIMER0_IMR_R = TIMER_IMR_TATOIM;            // Enable Timer 0 Interrupt
    WTIMER0_CTL_R |= TIMER_CTL_TAEN;             // Enable Timer 0
}

// Set the time of Wide Timer 0
void timer_setTime(uint64_t time)
{
    WTIMER0_TAILR_R = (uint32_t)(time & 0xFFFFFFFF);        // Load the lower 32 bits
    WTIMER0_TBILR_R = (uint32_t)(time >> 32);               // Load the upper 32 bits
}

// Get the current time of Wide Timer 0
uint64_t timer_getTime()
{
    uint64_t time = 0;
    time |= WTIMER0_TBILR_R;                               // Get the upper 32 bits
    time <<= 32;                                           // Shift to make space for the lower 32 bits
    time |= WTIMER0_TAILR_R;                              // Get the lower 32 bits
    return time;                                          // Return the combined 64-bit time
}

// Check if Wide Timer 0 has triggered an interrupt
bool timer_checkTime()
{
    return WTIMER0_RIS_R & TIMER_RIS_TATORIS;           // Check Timer A interrupt
}

// Clear the interrupt flag of Wide Timer 0
void timer_handle()
{
    WTIMER0_ICR_R = TIMER_ICR_TATOCINT;                // Clear Timer A interrupt
}
