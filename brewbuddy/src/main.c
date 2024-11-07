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
#include "tm4c123gh6pm.h"
#include "clock.h"
#include "heater.h"
#include "thermo.h"
#include "interface.h"
#include "tick.h"

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

uint32_t tp; // User Input Temperature
uint32_t tm; // Thermistor Temperature

//-----------------------------------------------------------------------------
// Hardware Initialize Function
//-----------------------------------------------------------------------------

void hardware_init(void)
{
    initSystemClockTo40Mhz();
    heater_init();
    thermo_init();
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

    // Initialize Global Variables
    tp = 0; // User Input Temperature
    tm = 0; // Thermistor Temperature

    // Initialize Hardware
    hardware_init();
    heater_off();

    // Endless loop
    while(true)
    {
        __asm("   NOP");
        inf_getCommand(&data);
        inf_parseCommand(&data);
        inf_doCommand(&data, &tp, tm);
    }
}

//-----------------------------------------------------------------------------
// System Tick Interrupt Service Routine (One Hz)
//-----------------------------------------------------------------------------

void tick_ISR()
{
    // Degree symbol for temperature symbol
    char degree_symbol[4] = {0xC2, 0xB0, 'F', '\0'};

    // Get Thermistor Temperature
    tm = thermo_getTEMP(thermo_getADC());

    // Check if Heater should be on or off
    if (tp <= tm)
        heater_off();
    else
        heater_on();

    // Print Thermistor Temperature if Debugging
//    inf_printUINT(tm);
//    inf_puts("\r\n");
//    inf_puts("\033[2J\033[H");   // Clear Screen
//    inf_puts("Thermistor Temperature: ");
//    inf_printUINT(tm);
//    inf_puts(degree_symbol);
    // inf_puts("\r\n");
    // inf_puts("\x1B[?25l"); 

    // Clear Interrupt Flag
    TIMER0_ICR_R = TIMER_ICR_TATOCINT; // clear interrupt flag
    return;
}
