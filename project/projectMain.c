#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "uart0.h"
#include "clock.h"
#include "gpio.h"
#include "nvic.h"
#include "wait.h"
#include "adc0.h"
#include "interface.h"
#include "tm4c123gh6pm.h"

#define PWM_PIN PORTF, 2

#define AIN3_MASK 1

float rawADC;

USER_DATA data;

void initHW()
{
    initSystemClockTo40Mhz();

    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1 | SYSCTL_RCGCTIMER_R0; // timer 1 periodic 1 second
    SYSCTL_RCGCWTIMER_R |= SYSCTL_RCGCWTIMER_R1;
    SYSCTL_RCGCPWM_R = SYSCTL_RCGCPWM_R1;
    _delay_cycles(3);

    enablePort(PORTC);
    enablePort(PORTE);
    enablePort(PORTF);

    selectPinPushPullOutput(PWM_PIN);


    selectPinAnalogInput(PORTE, 2);

    GPIO_PORTF_DEN_R |= 4;
    GPIO_PORTF_AFSEL_R |= 4;
    GPIO_PORTF_PCTL_R &= ~(GPIO_PCTL_PF2_M);
    GPIO_PORTF_PCTL_R |= GPIO_PCTL_PF2_M1PWM6;

    SYSCTL_SRPWM_R = SYSCTL_SRPWM_R1;                // reset PWM1 module
    SYSCTL_SRPWM_R = 0;                              // leave reset state
    PWM1_3_CTL_R = 0;                                // turn-off PWM1 generator 3 (drives outs 6 and 7)
    PWM1_3_GENA_R = PWM_1_GENA_ACTCMPAD_ONE | PWM_1_GENA_ACTLOAD_ZERO;
                                                     // output 6 on PWM1, gen 3a, cmpa

    PWM1_3_LOAD_R = 1024;                            // (internal counter counts down from load value to zero)
    PWM1_3_CMPA_R = 0;                               // blue off

    PWM1_3_CTL_R = PWM_1_CTL_ENABLE;                 // turn-on PWM1 generator 3
    PWM1_ENABLE_R |= PWM_ENABLE_PWM6EN;
}

void setPwm(uint16_t pwmVal){

    // divide pwm input by 100
    // multiply by load register
    // create intermediary variable
    // then set that equal to the compare register

    float pwmFloat = 0;

    pwmFloat = (float)pwmVal / 100;

    pwmFloat = pwmFloat * 1023;

    PWM1_3_CMPA_R = pwmFloat;
}

void heat(){
    // turn on the heating element in whatever capacity we do it with
}

int main(){

    initHW();

    initUart0();
    setUart0BaudRate(115200, 40e6);

    initAdc0Ss3();
    setAdc0Ss3Mux(1);
    setAdc0Ss3Log2AverageCount(2);

    displayMenu();
    while(true){

        getsUart0(&data);
        parseFields(&data);

        processCommands();
    }
}
