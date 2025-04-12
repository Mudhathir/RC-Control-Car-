/*
 * timersB0.c
 *
 *  Created on: Oct 1, 2024
 *      Author: fahimsharifkhaja
 */

#include "msp430.h"
#include "string.h"
#include "functions.h"
#include "LCD.h"
#include "ports.h"
#include "macros.h"
#include "moves.h"
#include "timersB0.h"
#include"hex2bdc.h"
#include "ADC.h"
#include "pwm.h"

extern volatile unsigned char update_display;
extern volatile unsigned int count_debounce_SW1;
extern volatile unsigned int count_debounce_SW2;
extern unsigned int iot_time;

int counttime;
int sw1_debounce_time = 0;
int sw2_debounce_time = 0;
int sw1_active = 0;
int sw2_active = 0;

void Init_Timers(void) {
    Init_Timer_B0();
    Init_Timer_B3();
}

//------------------------------------------------------------------------------
// Timer B0 initialization sets up both B0_0, B0_1-B0_2 and overflow
void Init_Timer_B0(void) {
    TB0CTL = TBSSEL__SMCLK;         // SMCLK source
    TB0CTL |= TBCLR;                // Resets TB0R, clock divider, count direction
    TB0CTL |= MC__CONTINUOUS;       // Continuous mode
    TB0CTL |= ID__2;                // Divide clock by 2

    TB0EX0 = TBIDEX__8;             // Divide clock by an additional 8

    TB0CCR0 = TB0CCR0_INTERVAL;     // Set CCR0 interval
    TB0CCTL0 |= CCIE;               // Enable interrupt for CCR0

    TB0CCR1 = TB0CCR1_INTERVAL;
    TB0CCTL1 |= CCIE;               // Enable interrupt for CCR1

    TB0CCR2 = TB0CCR2_INTERVAL;     // Set CCR2 interval for SW2 debounce
    TB0CCTL2 |= CCIE;               // Enable interrupt for CCR2

    TB0CTL &= ~TBIE;                // Disable Overflow Interrupt
    TB0CTL &= ~TBIFG;               // Clear Overflow Interrupt flag
}
//------------------------------------------------------------------------------

#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer0_B0_ISR(void) {
    counttime++;
    iot_time++;


    if (counttime == 10) { // Toggle backlight every 200ms updates display
        counttime = 0;
        update_display = 1;
    }

    ADCCTL0 |= ADCENC;
    ADCCTL0 |= ADCSC;  // Correct way to start an ADC conversion
    //P2OUT |= IR_LED;  // Assuming IR_LED is defined and this operation is correct

    TB0CCR0 += TB0CCR0_INTERVAL; // Add offset to CCR0
}

//----------------------------------------------------------------------------

#pragma vector = TIMER0_B1_VECTOR
__interrupt void TIMER0_B1_ISR(void) {
    switch (__even_in_range(TB0IV, 14)) {
        case 0: break; // No interrupt
        case 2: // SW1 Debounce (CCR1)
            sw1_debounce_time += 100; // Increment debounce time by 100ms

            if (sw1_debounce_time >= 1000) { // If 1 second has passed
                TB0CCTL1 &= ~CCIE; // Disable CCR1 interrupt
                P4IFG &= ~SW1;     // Clear SW1 interrupt flag
                P4IE |= SW1;       // Re-enable SW1 interrupt
                TB0CCTL0 |= CCIE;  // Re-enable CCR0 (backlight control)
                sw1_debounce_time = 0;
                sw1_active = 0;
            }
            break;
        case 4: // SW2 Debounce (CCR2)
            sw2_debounce_time += 100; // Increment debounce time by 100ms

            if (sw2_debounce_time >= 1000) { // If 1 second has passed
                TB0CCTL2 &= ~CCIE; // Disable CCR2 interrupt
                P2IFG &= ~SW2;     // Clear SW2 interrupt flag
                P2IE |= SW2;       // Re-enable SW2 interrupt
                TB0CCTL0 |= CCIE;  // Re-enable CCR0 (backlight control)
                sw2_debounce_time = 0;
                sw2_active = 0;
            }
            break;
        case 14: // Overflow (if needed, handle here)
            break;
        default: break;
    }
}
//----------------------------------------------------------------------------
void Init_Timer_B3(void) {
    //------------------------------------------------------------------------------
    // SMCLK source, up count mode, PWM Right Side
    // TB3.1 P6.0 LCD_BACKLITE
    // TB3.2 P6.1 R_FORWARD
    // TB3.3 P6.2 R_REVERSE
    // TB3.4 P6.3 L_FORWARD
    // TB3.5 P6.4 L_REVERSE
    //------------------------------------------------------------------------------
     TB3CTL = TBSSEL__SMCLK; // SMCLK
     TB3CTL |= MC__UP; // Up Mode
     TB3CTL |= TBCLR; // Clear TAR

     PWM_PERIOD = WHEEL_PERIOD; // PWM Period [Set this to 50005]

     TB3CCTL1 = OUTMOD_7; // CCR1 reset/set
     LCD_BACKLITE_BRIGHTNESS = PERCENT_80; // P6.0 Right Forward PWM duty cycle

     TB3CCTL2 = OUTMOD_7; // CCR2 reset/set
     RIGHT_FORWARD_SPEED = WHEEL_OFF; // P6.1 Right Forward PWM duty cycle

     TB3CCTL3 = OUTMOD_7; // CCR3 reset/set
     LEFT_FORWARD_SPEED = WHEEL_OFF; // P6.2 Left Forward PWM duty cycle

     TB3CCTL4 = OUTMOD_7; // CCR4 reset/set
     RIGHT_REVERSE_SPEED = WHEEL_OFF; // P6.3 Right Reverse PWM duty cycle

     TB3CCTL5 = OUTMOD_7; // CCR5 reset/set
     LEFT_REVERSE_SPEED = WHEEL_OFF; // P6.4 Left Reverse PWM duty cycle
    //------------------------------------------------------------------------------
}






