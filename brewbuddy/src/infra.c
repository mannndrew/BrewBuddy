// Infrared Library (Definition)
// File: infra.c
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

// Hardware configuration (I2C0):
// SCL - PB2
// SDA - PB3

//-----------------------------------------------------------------------------
// Device Includes
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "infra.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

#define MLX90614_ADDR 0x5A  // 7-bit address of the MLX90614

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize Hardware (I2C0)
void infra_init(void)
{
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;  // Enable GPIO port B
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1) == 0);
    
    SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R0;    // Enable I2C0
    while ((SYSCTL_PRI2C_R & SYSCTL_PRI2C_R0) == 0);

    GPIO_PORTB_AFSEL_R |= 0x03;  // Enable alternate function for PB2, PB3
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFFFF00) | 0x00000033; // Configure pins for I2C0
    GPIO_PORTB_DEN_R |= 0x03;    // Enable digital function on PB2, PB3
    GPIO_PORTB_PUR_R |= 0x03;    // Enable pull-ups on PB2, PB3

    I2C0_MCR_R = I2C_MCR_MFE;   // Set I2C0 as master
    I2C0_MTPR_R = 0x07;         // Set I2C speed to standard mode (100 kHz)
}

uint32_t infra_read(void)
{
    uint32_t msb;
    uint32_t lsb;
    uint32_t value;
    
    // Send register address to read
    I2C0_MSA_R = (MLX90614_ADDR << 1) & 0xFE; // Set address (write)
    I2C0_MDR_R = 0x6;                         // Set the register to read
    I2C0_MCS_R = I2C_MCS_RUN | I2C_MCS_START; // Start transmission
    
    while (I2C0_MCS_R & I2C_MCS_BUSY);  // Wait for transaction to complete
    
    // Now switch to read mode and get the data
    I2C0_MSA_R = (MLX90614_ADDR << 1) | 0x01; // Set address (read)
    I2C0_MCS_R = I2C_MCS_RUN | I2C_MCS_START;  // Start transmission
    
    while (I2C0_MCS_R & I2C_MCS_BUSY);  // Wait for transaction to complete
    
    msb = I2C0_MDR_R; // Read MSB
    I2C0_MCS_R = I2C_MCS_RUN;  // Complete read
    
    while (I2C0_MCS_R & I2C_MCS_BUSY);  // Wait for transaction to complete
    
    lsb = I2C0_MDR_R;  // Read LSB
    
    value = (msb << 8) | lsb;  // Combine MSB and LSB into 16-bit value
    
    return value;
}
