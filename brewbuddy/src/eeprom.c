// EEPROM Library (Definition)
// File: eeprom.c
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
#include <stdbool.h>
#include <driverlib/eeprom.h>
#include "tm4c123gh6pm.h"

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void eeprom_init()
{
    SYSCTL_RCGCEEPROM_R |= SYSCTL_RCGCEEPROM_R0;    // Enable Clock Gating for EEPROM Module
    EEPROMInit();
}

void eeprom_write(uint32_t *data)
{
    EEPROMProgram(data, 0, 4);
}

void eeprom_read(uint32_t *data)
{
    EEPROMRead(data, 0, 4);
}
