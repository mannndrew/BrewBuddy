#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "uart0.h"
#include "tm4c123gh6pm.h"

#ifndef INTERFACE_H_
#define INTERFACE_H_

void displayMenu();
void processCommands();

#endif
