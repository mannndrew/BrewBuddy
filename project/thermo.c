#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "thermo.h"

/*int main(void){
    int i;
    if(i=0; i<25; i++){
        printf("Temperature: %d, Ohms: %d\n", tempF.tempF[i], ohms.ohms[i]);
    }
}*/

void thermo_init(void){
    Lut.ohms = {37660, 35976, 34376, 32843, 31399, 28722, 27481, 26300, 24107, 22111, 20308, 
            18670, 17180, 15824, 14584, 13458, 12932, 12430, 11490, 11051, 10627, 9841, 9473, 9121, 8783};
    Lut.tempF = {117, 119, 121, 123, 125, 129, 131, 133, 137, 141, 145, 149, 153, 157, 161, 165, 167, 169, 173, 175, 177, 181, 183, 185, 187};
}

void thermo_setTemp(int temp){
    int prev, next;
    for(int i = 0; i < MAX; i++){
        // setting the previous and next temp values
        if(i == 0){
            prev = 0;
            next = Lut.tempF[i+1];
        } else if(i == MAX - 1){
            prev = Lut.tempF[i-1];
            next = Lut.tempF[i];
        } else {
            prev = Lut.tempF[i-1];
            next = Lut.tempF[i+1];
        }

        if(temp == Lut.tempF[i]){
            Lut.index = i;
            //turn on the themro
            return;
        } else if ((temp < next) & (temp > prev)){
            Lut.index = i-1;
            // turn on the thermo
            return;
        }
    }
}

int thermo_getTemp(int Vout){
    int currentOhms = 0;
    int rOhms = 9.89*1000;
    currentOhms = (3.3*rOhms)/Vout - rOhms;
    if(currentOhms >= Lut.ohms[Lut.index]){
        // Turn off the themro
        return Lut.tempF[Lut.index];
    } else {

    }
}