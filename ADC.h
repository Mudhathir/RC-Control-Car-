/*
 * ADC.h
 *
 *  Created on: Oct 17, 2024
 *      Author: fahimsharifkhaja
 */

#ifndef ADC_H_
#define ADC_H_

#include "msp430.h"

extern volatile unsigned int ADC_Left_Det;
extern volatile unsigned int ADC_Right_Det;
extern volatile unsigned int ADC_Thumb;
extern volatile unsigned char ADC_Channel;

void Init_ADC(void);
__interrupt void ADC_ISR(void);

#endif /* ADC_H_ */
