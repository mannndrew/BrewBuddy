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
#include "infra.h"
#include "interface.h"
#include "tick.h"
#include "wait.h"

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

uint32_t tp; // User Input Temperature
uint32_t tm; // Thermistor Temperature
uint32_t ti; // Infrared Temperature

//-----------------------------------------------------------------------------
// Hardware Initialize Function
//-----------------------------------------------------------------------------

void hardware_init(void)
{
    initSystemClockTo40Mhz();
    heater_init();
    thermo_init();
    inf_init();
    infra_init();
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

    // Initialize Hardware
    hardware_init();
    heater_off();
    inf_boldOn();

    ///////////// TEST CODE ///////////
    /*
    uint32_t value;
    infra_init();
    waitMicrosecond(2000);

    while (1)
    {
        value = infra_read();
        inf_printUINT(value);
        inf_puts("\r\n");

        // delay
        waitMicrosecond(1000000);
    }
    */
    ///////////////////////////////////

    // Endless loop
    while(true)
    {
        inf_setCursor(5);
        inf_cursorOn();
        inf_getCommand(&data);
        inf_cursorOff();
        inf_clearScreen(4, 10);
        inf_setCursor(6);
        inf_parseCommand(&data);
        inf_doCommand(&data, &tp, tm);
    }
}

//-----------------------------------------------------------------------------
// System Tick Interrupt Service Routine (One Hz)
//-----------------------------------------------------------------------------

void tick_ISR()
{
    // Get Infrared Temperature
    ti = infra_read();

    // Get Thermistor Temperature
    tm = thermo_getTEMP(thermo_getADC());

    // Check if Heater should be on or off
    if (tp <= tm)
        heater_off();
    else
        heater_on();

    // Print Header
    inf_printHeader(ti, tm, tp);

    // Clear Interrupt Flag
    TIMER0_ICR_R = TIMER_ICR_TATOCINT; // clear interrupt flag
    return;
}
