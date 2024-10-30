// System Tick Library (Definition)
// File: tick.c
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

//-----------------------------------------------------------------------------
// Device Includes
//-----------------------------------------------------------------------------

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "tick.h"

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize System Tick (Timer0)
void tick_init()
{
    // Enable clocks with 3 wait cycles
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0; // Enable Timer 0 clock
    __asm("   NOP"); // Wait cycle 1
    __asm("   NOP"); // Wait cycle 2
    __asm("   NOP"); // Wait cycle 3

    // Configure Timer for State Switch (Timer 0 Subtimer A)
    TIMER0_CTL_R &= ~TIMER_CTL_TAEN;            // Disable Timer 0
    TIMER0_CFG_R = TIMER_CFG_32_BIT_TIMER;      // Set Timer 0 as 32-bit timer
    TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;     // Set Timer 0 to Periodic Mode
    TIMER0_TAILR_R = 40000000;                  // Set Timer 0 to 1 Hz
    TIMER0_IMR_R = TIMER_IMR_TATOIM;            // Enable Timer 0 Interrupt
    TIMER0_CTL_R |= TIMER_CTL_TAEN;             // Enable Timer 0
    NVIC_EN0_R |= 1 << (INT_TIMER0A-16);        // Enable Timer 0 Interrupt
}
