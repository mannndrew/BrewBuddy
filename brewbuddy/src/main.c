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
#include "button.h"
#include "eeprom.h"
#include "heater.h"
#include "thermo.h"
#include "infra.h"
#include "interface.h"
#include "lcd.h"
#include "tick.h"
#include "wait.h"

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

uint32_t tp;    // User Input Temperature
uint32_t tm;    // Thermistor Temperature
uint32_t ti;    // Infrared Temperature
uint32_t tsafe; // Safety Temperature

//-----------------------------------------------------------------------------
// Hardware Initialize Function
//-----------------------------------------------------------------------------

void hardware_init(void)
{
    initSystemClockTo40Mhz();
    button_init();
    eeprom_init();
    heater_init();
    thermo_init();
    inf_init();
    infra_init();
    lcd_init();
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
    eeprom_read(&tsafe);
    tp = 70;

    // Endless loop
    while(true)
    {
        inf_setCursor(6);
        inf_cursorOn();
        inf_getCommand(&data);
        inf_cursorOff();
        inf_clearScreen(5, 12);
        inf_setCursor(7);
        inf_parseCommand(&data);
        inf_doCommand(&data, &tp, &tsafe);
    }
}

//-----------------------------------------------------------------------------
// System Tick Interrupt Service Routine (One Hz)
//-----------------------------------------------------------------------------

void tick_ISR()
{
    bool heater_status;

    if (button_status())
    {
        if (tp == 140)
            tp = 70;
        else
            tp += 10 - (tp % 10);
    }
    

    // Get Infrared Temperature
    ti = infra_read();

    // Get Thermistor Temperature
    tm = thermo_getTEMP(thermo_getADC());

    // Check if Heater should be on or off
    heater_status = (tm < tsafe && ti < tp);
    if (heater_status)
        heater_on();
    else
        heater_off();

    // Print Header
    inf_printHeader(ti, tm, tp, heater_status);

    // Print LCD
    lcd_print(ti, tm, tp, heater_status);

    // Clear Interrupt Flag
    TIMER0_ICR_R = TIMER_ICR_TATOCINT; // clear interrupt flag
    return;
}
