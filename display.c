/*
 * display.c
 *
 *  Created on: Sep 17, 2024
 *      Author: fahimsharifkhaja
 */

#include "msp430.h"

extern volatile unsigned char update_display;
extern volatile unsigned char display_changed;


void Display_Process(void){
  if(update_display){
    update_display = 0;
    if(display_changed){
      display_changed = 0;
      Display_Update(0,0,0,0);
    }
  }
}
