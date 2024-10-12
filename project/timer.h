// Timer Library (Header)
// File: timer.h

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL Evaluation Board
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

//-----------------------------------------------------------------------------
// Preprocessor Directives
//-----------------------------------------------------------------------------

#ifndef TIMER_H_
#define TIMER_H_

//-----------------------------------------------------------------------------
// Device Includes
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Set the time of Wide Timer 0
void timer_setTime(uint64_t time);

// Get the current time of Wide Timer 0
uint64_t timer_getTime();

// Check if Wide Timer 0 has triggered an interrupt
bool timer_checkTime();

// Clear the interrupt flag of Wide Timer 0
void timer_handle();

#endif
