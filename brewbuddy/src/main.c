// BrewBuddy Main File
// File: main.c
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

#include <stdbool.h>
#include "clock.h"
#include "interface.h"
#include "tick.h"
#include "tm4c123gh6pm.h"

//-----------------------------------------------------------------------------
// Hardware Initialize Function
//-----------------------------------------------------------------------------

void hardware_init(void)
{
    initSystemClockTo40Mhz();
    inf_init();
    tick_init();
    return;
}

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

int main(void)
{
    // Initialize Local Variables
    USER_DATA data;  // User data struct for UART
    uint32_t tp = 0;  // User temperature input

    // Initialize Hardware
    hardware_init();

    // Endless loop
    while(true)
    {
        __asm("   NOP");
        inf_getCommand(&data);
        inf_parseCommand(&data);
        inf_doCommand(&data, &tp);
    }
}

//-----------------------------------------------------------------------------
// System Tick Interrupt Service Routine (One Hz)
//-----------------------------------------------------------------------------

void tick_ISR()
{
    TIMER0_ICR_R = TIMER_ICR_TATOCINT; // clear interrupt flag
    return;
}
