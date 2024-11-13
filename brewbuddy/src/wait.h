// Wait Library (Header)
// File: wait.h
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
// Preprocessor Directives
//-----------------------------------------------------------------------------

#ifndef WAIT_H_
#define WAIT_H_

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

extern void waitMicrosecond(unsigned int us);

#endif
