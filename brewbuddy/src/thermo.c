// Thermistor Library (Definition)
// File: thermo.c
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

// Hardware configuration:
// ADC0 SS3

//-----------------------------------------------------------------------------
// Device Includes
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "tm4c123gh6pm.h"
#include "thermo.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

#define ADC_CTL_DITHER          0x00000040
#define AIN3_MASK               1

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize Hardware
void thermo_init()
{
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4; // Enable Port E clock
    SYSCTL_RCGCADC_R |= SYSCTL_RCGCADC_R0;   // Enable ADC0 clock
    __asm("   NOP"); // Wait cycle 1
    __asm("   NOP"); // Wait cycle 2
    __asm("   NOP"); // Wait cycle 3

    // Configure AIN3 as an analog input (PE0)
	GPIO_PORTE_AFSEL_R |= AIN3_MASK;                 // select alternative functions for AIN3 (PE0)
    GPIO_PORTE_DEN_R &=  ~AIN3_MASK;                 // turn off digital operation on pin PE0
    GPIO_PORTE_AMSEL_R |= AIN3_MASK;                 // turn on analog operation on pin PE0

    // Configure ADC
    ADC0_ACTSS_R &= ~ADC_ACTSS_ASEN3;                // disable sample sequencer 3 (SS3) for programming
    ADC0_CC_R = ADC_CC_CS_SYSPLL;                    // select PLL as the time base (not needed, since default value)
    ADC0_PC_R = ADC_PC_SR_1M;                        // select 1Msps rate
    ADC0_EMUX_R = ADC_EMUX_EM3_PROCESSOR;            // select SS3 bit in ADCPSSI as trigger
    ADC0_SSCTL3_R = ADC_SSCTL3_END0;                 // mark first sample as the end
    ADC0_SSMUX3_R = 3;                               // set first sample to AIN3
    ADC0_SAC_R = 4;                                  // sample HW averaging
    ADC0_CTL_R |= ADC_CTL_DITHER;                     // turn-on dithering if averaging
    ADC0_ACTSS_R |= ADC_ACTSS_ASEN3;                 // enable SS3 for operation
}

// Request and read one sample from SS3
uint32_t thermo_getADC()
{
    ADC0_PSSI_R |= ADC_PSSI_SS3;                     // set start bit
    while (ADC0_ACTSS_R & ADC_ACTSS_BUSY);           // wait until SS3 is not busy
    while (ADC0_SSFSTAT3_R & ADC_SSFSTAT3_EMPTY);    // wait if FIFO is empty
    return ADC0_SSFIFO3_R;                           // get single result from the FIFO
}

// Convert ADC sample to temperature
uint32_t thermo_getTEMP(uint32_t ADC)
{
    float r1 = 10000;  // dummy 10k ohm resistor
    float r0 = 100000; // thermistor resistance at 25 degrees Celsius
    float t0 = 298.15; // 25 degrees Celsius in Kelvin
    float b0 = 3950;   // thermistor beta value

    float tvol; // thermistor voltage
    float tres; // thermistor resistance
    float ttem; // temperature in Celsius

    // Convert ADC sample to voltage
    tvol = (ADC/ 4095) * 3.3;

    // Convert voltage to resistance
    tres = ((r1 * 3.3) / tvol) - r1;

    // Convert resistance to temperature
    ttem = 1 / ((1 / t0) + (1 / b0) * log(tres / r0));

    return (uint32_t) ttem;
}
