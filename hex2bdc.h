/*
 * hex2bdc.h
 *
 *  Created on: Oct 17, 2024
 *      Author: fahimsharifkhaja
 */

#ifndef HEXTOBCD_H_
#define HEXTOBCD_H_

#include "msp430.h"

unsigned int adc_char[4];  // Array to store BCD characters

void HEXtoBCD(int hex_value);
void adc_line(char line, char location);

#endif /* HEXTOBCD_H_ */


