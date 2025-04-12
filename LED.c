/*
 * LED.c
 *
 *  Created on: Sep 17, 2024
 *      Author: fahimsharifkhaja
 */

#include "msp430.h"
#include "ports.h"

void Init_LEDs(void){
//------------------------------------------------------------------------------
// LED Configurations
//------------------------------------------------------------------------------
// Turns on both LEDs
  P1OUT &= ~RED_LED;
  P6OUT &= ~GRN_LED;
//------------------------------------------------------------------------------
}

