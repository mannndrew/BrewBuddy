// Analog Example
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL with LCD/Temperature Sensor
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz
// Stack:           4096 bytes (needed for snprintf)

// Hardware configuration:
// LM60 Temperature Sensor:
//   AIN3/PE0 is driven by the sensor
//   (V = 424mV + 6.25mV / degC with +/-2degC uncalibrated error)
// UART Interface:
//   U0TX (PA1) and U0RX (PA0) are connected to the 2nd controller
//   The USB on the 2nd controller enumerates to an ICDI interface and a virtual COM port
//   Configured to 115,200 baud, 8N1

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "clock.h"
#include "wait.h"
#include "uart0.h"
#include "adc0.h"
#include "tm4c123gh6pm.h"

// PortE masks
#define AIN3_MASK 1

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize Hardware
void initHw()
{
    // Initialize system clock to 40 MHz
    initSystemClockTo40Mhz();

    // Enable clocks
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;
    _delay_cycles(3);

    // Configure AIN3 as an analog input
	GPIO_PORTE_AFSEL_R |= AIN3_MASK;                 // select alternative functions for AIN3 (PE0)
    GPIO_PORTE_DEN_R &= ~AIN3_MASK;                  // turn off digital operation on pin PE0
    GPIO_PORTE_AMSEL_R |= AIN3_MASK;                 // turn on analog operation on pin PE0
}

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

void numSTR(uint32_t num, char* str)
{
    // Index for storing characters
    int i = 0;

    // Handle the case for 0 explicitly since the logic below won't catch it
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0'; // Null-terminate the string
        return;
    }

    // Temporary array to hold digits in reverse order
    char temp[10];
    int len = 0;

    // Extract digits from the number
    while (num > 0)
    {
        temp[len++] = (num % 10) + '0'; // Get the last digit and convert to character
        num /= 10;                      // Remove the last digit from the number
    }

    // Reverse the digits to get the correct order
    while (len > 0)
    {
        str[i++] = temp[--len];
    }

    // Null-terminate the string
    str[i] = '\0';
}

int main(void)
{
    uint32_t adc_value;
    float voltage;
    uint32_t integer_part;
    uint32_t fraction_part;

    char str1[80];
    char str2[80];

    // Initialize hardware
    initHw();
    initUart0();
    initAdc0Ss3();

    // Setup UART0 baud rate
    setUart0BaudRate(115200, 40e6);

    // Use AIN3 input with N=16 hardware sampling
    setAdc0Ss3Mux(3);
    setAdc0Ss3Log2AverageCount(4);


    // Endless loop
    while(true)
    {
        // Read sensor
        adc_value = readAdc0Ss3();
        voltage = (adc_value / 4095.0) * 3.3;
        integer_part = (uint32_t)voltage;
        fraction_part = (uint32_t)((voltage - integer_part) * 1000);
        numSTR(integer_part, str1);
        numSTR(fraction_part, str2);

        // display raw ADC value and temperatures
        putsUart0(str1);
        putsUart0(".");
        putsUart0(str2);
        putsUart0("\r\n");
        waitMicrosecond(1000000);
    }
}
