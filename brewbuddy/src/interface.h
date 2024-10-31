// UART Interface Library (Header)
// File: interface.h
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
// UART Interface:
//   U0TX (PA1) and U0RX (PA0) are connected to the 2nd controller
//   The USB on the 2nd controller enumerates to an ICDI interface and a virtual COM port

//-----------------------------------------------------------------------------
// Preprocessor Directives
//-----------------------------------------------------------------------------

#ifndef INTERFACE_H_
#define INTERFACE_H_

//-----------------------------------------------------------------------------
// Device Includes
//-----------------------------------------------------------------------------

#include <stdint.h>

//-----------------------------------------------------------------------------
// Device Defines
//-----------------------------------------------------------------------------

#define MAX_CHARS 80
#define MAX_FIELDS 6

//-----------------------------------------------------------------------------
// Structs
//-----------------------------------------------------------------------------

typedef struct _USER_DATA
{
    char buffer[MAX_CHARS+1];
    uint8_t fieldCount;
    uint8_t fieldPosition[MAX_FIELDS];
    char fieldType[MAX_FIELDS];
} USER_DATA;

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

uint32_t inf_getUINT(USER_DATA *data, uint8_t argc);
void inf_printUINT(uint32_t num);

bool inf_isNumber(char *str, uint8_t *len);
bool inf_isAlpha(char *str, uint8_t *len);
bool inf_isCombo(char *str, uint8_t *len);
bool inf_isCommand(USER_DATA *data, const char cmd[], uint8_t argc);
bool inf_strCompare(char str1[], char str2[]);

void inf_init();
void inf_putc(char c);
void inf_puts(char *str);
char inf_getc();
void inf_getCommand(USER_DATA *data);
void inf_parseCommand(USER_DATA *data);
void inf_doCommand(USER_DATA *data, uint32_t *tp, uint32_t tm);

#endif
