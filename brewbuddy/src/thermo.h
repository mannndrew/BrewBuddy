// Thermistor Library (Header)
// File: thermo.c
// Group Members:
// - Andrew Howard
// - James Revette
// - Madison Gage

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:
// ADC0 SS3

//-----------------------------------------------------------------------------
// Preprocessor Directives
//-----------------------------------------------------------------------------

#ifndef THERMO_H_
#define THERMO_H_

//-----------------------------------------------------------------------------
// Device Includes
//-----------------------------------------------------------------------------

#include <stdint.h>

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void thermo_init();
uint32_t thermo_getADC();
uint32_t thermo_getTEMP(uint32_t ADC);

#endif
