// Button Library (Header)
// File: button.h
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

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void button_init(void);   // Initialize Button
bool button_status(void); // Get Button Status
