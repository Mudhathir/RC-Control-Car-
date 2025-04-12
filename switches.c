#include "msp430.h"
#include <string.h>
#include "functions.h"
#include "LCD.h"
#include "ports.h"
#include "macros.h"
#include "switches.h"
#include "timersB0.h"
#include "hardware.h"

// External Variables (ensure these are declared elsewhere in your project)
extern char display_line[4][11];
extern volatile unsigned char display_changed;
//extern volatile unsigned int update_display;
extern int backlight;
extern char IOT_Ring_Rx[11];
extern unsigned int iot_rx_wr;
extern volatile unsigned int sw1_position;
extern volatile unsigned int sw2_position;
 unsigned int count_debounce_SW1;
 unsigned int count_debounce_SW2;

// Interrupt Service Routine for PORT4 (Switch 1)
#pragma vector=PORT4_VECTOR
__interrupt void switchP4_interrupt(void) {
    if (P4IFG & SW1) {  // Check if SW1 caused the interrupt
        P4IFG &= ~SW1;  // Clear interrupt flag for SW1
        P4IE &= ~SW1;   // Disable interrupt for SW1 for debounce handling

        // Clear IOT_Ring_Rx buffer
        memset(IOT_Ring_Rx, ' ', sizeof(IOT_Ring_Rx));
        IOT_Ring_Rx[10] = '\0';  // Null-terminate the string
        iot_rx_wr = 0;
//        UCA0TXBUF = 'u';

        TB0CCR1 = TB0R + SW1_DEBOUNCE_TIME;  // Set debounce timer
        TB0CCTL1 |= CCIE;                    // Enable interrupt for debounce
        TB0CCTL0 &= ~CCIE;                   // Disable other Timer B interrupts
        sw1_position = 1;
        count_debounce_SW1 = 0;
    }
}

// Interrupt Service Routine for PORT2 (Switch 2)
#pragma vector=PORT2_VECTOR
__interrupt void switchP2_interrupt(void) {
    if (P2IFG & SW2) {  // Check if SW2 caused the interrupt
        P2IFG &= ~SW2;  // Clear interrupt flag for SW2
        P2IE &= ~SW2;   // Disable interrupt for SW2 for debounce handling

        // Clear IOT_Ring_Rx buffer
        memset(IOT_Ring_Rx, ' ', sizeof(IOT_Ring_Rx));
        IOT_Ring_Rx[10] = '\0';  // Null-terminate the string
        iot_rx_wr = 0;

        TB0CCR2 = TB0R + SW2_DEBOUNCE_TIME;  // Set debounce timer
        TB0CCTL2 |= CCIE;                    // Enable interrupt for debounce
        TB0CCTL0 &= ~CCIE;                   // Disable other Timer B interrupts
        sw2_position = 1;
        count_debounce_SW2 = 0;
    }
}
