#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "interface.h"
#include "uart0.h"
#include "tm4c123gh6pm.h"

char buffer[40];
int minutes = 0;
int seconds = 0;
int temp = 0;

USER_DATA data;

void displayMenu(){
    putsUart0("\nMenu:\n");
    putsUart0("Set Temperature (input temperature range) (Command: set temp num) \n");
    putsUart0("Set Timer (Command: set timer ##:##)\n");
    putsUart0("Check Timer (Command: disp timer)\n");
    putsUart0("Display current temperature (Command: disp temp)\n");
    putsUart0("Enter menu to display this menu again\n");
}

void processCommands(){
    if(isCommand(&data, "menu", 0)){
        displayMenu();
    }   

    if(isCommand(&data, "set", 3)){
        //printing the time
        //print the CLD register
        // if field type 1 and 2 are N, add to the register
        if(data.fieldType[2] == 'N' && data.fieldType[3] == 'N'){
            minutes = getFieldInteger(&data, 2);
            seconds = getFieldInteger(&data, 3);
            snprintf(buffer, sizeof(buffer), "\nSetting timer to: %d:%d \n", minutes, seconds);
            putsUart0(buffer);
        }
        // if field 1 is null print out the stored value
    } 
    else if(isCommand(&data, "set", 2)){
        // set temperature to the number
        temp = getFieldInteger(&data, 2);
        snprintf(buffer, sizeof(buffer), "\nSetting temperature to: %d \n", temp);
        putsUart0(buffer);
    }
    if(isCommand(&data, "disp", 1)){
        char* displayWhat = getFieldString(&data, 1);
        if(strcmp(displayWhat, "temp") == 0){
            // display the current temperature
            snprintf(buffer, sizeof(buffer), "\nCurrent temperature: %d \n", temp);
            putsUart0(buffer);
        }
        else if(strcmp(displayWhat, "timer") == 0){
            // display the current timer
            snprintf(buffer, sizeof(buffer), "\nCurrent timer: %d:%d \n", minutes, seconds);
            putsUart0(buffer);
        }
    }
}
