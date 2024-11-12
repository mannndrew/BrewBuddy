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

#define I2C0SCL (1 << 2)
#define I2C0SDA (1 << 3)
#define MLX90614_ADDR 0x5A  // 7-bit address of the MLX90614

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize Hardware (I2C0)
void infra_init(void)
{
    // Enable Clocks
    SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R0;    // Enable I2C0 Clock
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;  // Enable GPIOB Clock
    _delay_cycles(3);                         // Delay for clock to stabilize

    // Configure Push Pull Output for SCL
    GPIO_PORTB_ODR_R &= ~I2C0SCL;  // Set SCL to no open drain
    GPIO_PORTB_DIR_R |=  I2C0SCL;  // Set SCL as output
    GPIO_PORTB_DEN_R |=  I2C0SCL;  // Set SCL as digital

    // Configure Alternate Function for SCL
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFFF0FF) | GPIO_PCTL_PB2_I2C0SCL; // Set Port Control for SCL
    GPIO_PORTB_AFSEL_R |= I2C0SCL;  // Enable alternate function for SCL

    // Configure Open Drain Output for SDA
    GPIO_PORTB_ODR_R |= I2C0SDA;  // Set SDA to open drain
    GPIO_PORTB_DIR_R |= I2C0SDA;  // Set SDA as output
    GPIO_PORTB_DEN_R |= I2C0SDA;  // Set SDA as digital

    // Configure Alternate Function for SDA
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFF0FFF) | GPIO_PCTL_PB3_I2C0SDA; // Set Port Control for SDA
    GPIO_PORTB_AFSEL_R |= I2C0SDA;  // Enable alternate function for SDA

    // Configure I2C0 Peripheral
    I2C0_MCR_R = 0;                 // Disable I2C0 to program
    I2C0_MTPR_R = 19;               // Set I2C speed to standard mode (100 kHz)
    I2C0_MCR_R = I2C_MCR_MFE;       // Set I2C0 as master
    I2C0_MCS_R = I2C_MCS_STOP;      // Set I2C0 to stop
}

uint32_t infra_read(void)
{
    uint32_t msb;
    uint32_t lsb;
    uint32_t value;
    
    // Send register address to read
    I2C0_MSA_R = (MLX90614_ADDR << 1) & 0xFE; // Set address (write)
    I2C0_MDR_R = 0x7;                         // Set the register to read
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
