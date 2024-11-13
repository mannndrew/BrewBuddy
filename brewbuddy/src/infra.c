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

/*
#include "wait.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/i2c.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"
*/

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

#define I2C0SCL               (1 << 2)
#define I2C0SDA               (1 << 3)

#define MLX90614_ADDR         0x5A
#define MLX90614_AMBIENT_REG  0x06
#define MLX90614_OBJECT_REG   0x07

#define I2C_MASTER_CMD_BURST_SEND_START     0x00000003
#define I2C_MASTER_CMD_BURST_RECEIVE_START  0x0000000b
#define I2C_MASTER_CMD_BURST_RECEIVE_FINISH 0x00000005

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
    

    // Configure GPIO for SDA
    GPIO_PORTB_DEN_R |= I2C0SDA;
    GPIO_PORTB_AFSEL_R |= I2C0SDA;
    GPIO_PORTB_ODR_R |= I2C0SDA;
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFF0FFF) | (3 << 12);

    // Configure GPIO for SCL
    GPIO_PORTB_DEN_R |= I2C0SCL;
    GPIO_PORTB_AFSEL_R |= I2C0SCL;
    GPIO_PORTB_ODR_R &= ~I2C0SCL;
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFFF0FF) | (3 << 8);

    I2C0_MCR_R |= I2C_MCR_MFE;      // Enable I2C0 Master Function
    I2C0_MTPR_R = 19;               // Set I2C speed to standard mode (100 kHz) 
}

uint32_t infra_read(void)
{
    uint16_t data;
    float C, F; 

    I2C0_MSA_R = (MLX90614_ADDR << 1) & ~0x01;                   // Set address (write)
    I2C0_MDR_R = MLX90614_OBJECT_REG;                            // Set the register
    I2C0_MCS_R = I2C_MASTER_CMD_BURST_SEND_START;                // Send start condition
    while (I2C0_MCS_R & I2C_MCS_BUSY);                           // Wait for completion

    I2C0_MSA_R = (MLX90614_ADDR << 1) | 0x01;                    // Set address (read)
    I2C0_MCS_R = I2C_MASTER_CMD_BURST_RECEIVE_START;             // Send start condition
    while (I2C0_MCS_R & I2C_MCS_BUSY);                           // Wait for completion
    data = I2C0_MDR_R;                                           // MSB first

    I2C0_MCS_R = I2C_MASTER_CMD_BURST_RECEIVE_FINISH;            // Send finish condition
    while (I2C0_MCS_R & I2C_MCS_BUSY);                           // Wait for completion
    data |= (I2C0_MDR_R << 8);                                   // LSB second

    C = (data * 0.02) - 273.15;
    F = (C * 9.0/5.0) + 32.0;
    return (uint32_t) F;
}

/*
void infra_init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);

    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);
    I2CMasterEnable(I2C0_BASE);
}

uint32_t infra_read(void)
{
    uint16_t data;
    float C, F; 

    I2CMasterSlaveAddrSet(I2C0_BASE, MLX90614_ADDR, false); // Set to write mode
    I2CMasterDataPut(I2C0_BASE, 0x07);                       // Specify register
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);

    while (I2CMasterBusy(I2C0_BASE));                       // Wait for completion

    I2CMasterSlaveAddrSet(I2C0_BASE, MLX90614_ADDR, true);  // Set to read mode
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
    
    while (I2CMasterBusy(I2C0_BASE));                       // Wait for completion
    data = I2CMasterDataGet(I2C0_BASE);            // MSB first

    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

    while (I2CMasterBusy(I2C0_BASE));
    data |= (I2CMasterDataGet(I2C0_BASE) << 8);             // LSB second

    C = (data * 0.02) - 273.15;
    F = (C * 9.0/5.0) + 32.0;
    return (uint32_t) data;
}
*/
