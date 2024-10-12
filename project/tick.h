// Tick Library (Header)
// File: tick.h

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL Evaluation Board
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

//-----------------------------------------------------------------------------
// Preprocessor Directives
//-----------------------------------------------------------------------------

#ifndef TICK_H_
#define TICK_H_

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize Timer 0 for System Tick
void tick_init();

// Clear Timer 0 Interrupt
void tick_handle();

#endif
