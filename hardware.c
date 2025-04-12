/*
 * hardware.c
 *
 *  Created on: Oct 31, 2024
 *      Author: fahimsharifkhaja
 */

#include "msp430.h"
#include <string.h>
#include "LCD.h"
#include "ports.h"
#include "switches.h"
#include "timersB0.h"
#include "ADC.h"
#include "hardware.h"

// Define global variables for ring buffers and indexes
char IOT_Ring_Rx[11] = {0};
char iot_TX_buf[11] = {0};
unsigned int iot_tx = 0;
unsigned int iot_rx_wr = 0;
char USB_Ring_Rx[11] = {0};
char usb_TX_buf[11] = {0};
unsigned int usb_tx = 0;
unsigned int usb_rx_wr = 0;
unsigned int direct_iot = 0; // Declare direct_iot as a global variable
unsigned int iot_boot_timer = 0; // Define and initialize iot_boot_timer


// Define BEGINNING as 0 if not already defined
#ifndef BEGINNING
#define BEGINNING 0
#endif

void Init_Serial_UCA0(char speed) {
    // Configure eUSCI_A0 for UART mode
    UCA0CTLW0 = UCSWRST; // Put eUSCI in reset
    UCA0CTLW0 |= UCSSEL__SMCLK; // Set SMCLK as fBRCLK
    UCA0CTLW0 &= ~(UCMSB | UCSPB | UCPEN | UCSYNC | UC7BIT); // Configure control bits
    UCA0CTLW0 |= UCMODE_0; // Set to UART mode
    UCA0BRW = 4; // Set baud rate for 115,200
    UCA0MCTLW = 0x5551; // Configure modulation control
    UCA0CTLW0 &= ~UCSWRST; // Release from reset
    UCA0TXBUF = 0x00; // Prime the pump
    UCA0IE |= UCRXIE; // Enable RX interrupt
}

#pragma vector = EUSCI_A0_VECTOR
__interrupt void eUSCI_A0_ISR(void) { // This interrupt relates to serial communication port UCA0
    char iot_receive;
    switch(__even_in_range(UCA0IV, 0x08)) {
        case 0: break; // Vector 0 - no interrupt
        case 2: { // Vector 2 – Rx0IFG
            iot_receive = UCA0RXBUF;
            IOT_Ring_Rx[iot_rx_wr++] = iot_receive; // Add to Ring Buffer
            if (iot_rx_wr >= sizeof(IOT_Ring_Rx)) {
                iot_rx_wr = BEGINNING;
            }
            UCA1TXBUF = iot_receive; // Transmit to UCA1
        } break;
        case 4: { // Vector 4 – Tx0IFG
            UCA0TXBUF = iot_TX_buf[iot_tx];
            iot_TX_buf[iot_tx++] = 0;
            if (iot_TX_buf[iot_tx] == 0x00) {
                UCA0IE &= ~UCTXIE; // Disable TX interrupt if buffer is empty
                iot_tx = 0; // Reset index
            }
        } break;
        default: break;
    }
}


void Init_Serial_UCA1(char speed) {
    // Configure eUSCI_A1 for UART mode
    UCA1CTLW0 = UCSWRST; // Put eUSCI in reset
    UCA1CTLW0 |= UCSSEL__SMCLK; // Set SMCLK as fBRCLK
    UCA1CTLW0 &= ~(UCMSB | UCSPB | UCPEN | UCSYNC | UC7BIT); // Configure control bits
    UCA1CTLW0 |= UCMODE_0; // Set to UART mode
    UCA1BRW = 4; // Set baud rate for 115,200
    UCA1MCTLW = 0x5551; // Configure modulation control
    UCA1CTLW0 &= ~UCSWRST; // Release from reset
    UCA1TXBUF = 0x00; // Prime the pump
    UCA1IE |= UCRXIE; // Enable RX interrupt
}

#pragma vector = EUSCI_A1_VECTOR
__interrupt void eUSCI_A1_ISR(void) { // This interrupt relates to serial communication port UCA1
    char usb_value;
    switch(__even_in_range(UCA1IV, 0x08)) {
        case 0: break; // Vector 0 - no interrupt
        case 2: { // Vector 2 - Rx1IFG
            usb_value = UCA1RXBUF;
            USB_Ring_Rx[usb_rx_wr++] = usb_value; // Add to Ring Buffer
            if (usb_rx_wr >= sizeof(USB_Ring_Rx)) {
                usb_rx_wr = BEGINNING;
            }
            UCA0TXBUF = usb_value; // Transmit to UCA0
        } break;
        case 4: { // Vector 4 - TX1IFG
            UCA1TXBUF = IOT_Ring_Rx[direct_iot++];
            if (direct_iot >= sizeof(IOT_Ring_Rx)) {
                direct_iot = BEGINNING;
            }
            if (iot_rx_wr == direct_iot) {
                UCA1IE &= ~UCTXIE; // Disable TX interrupt if buffer is empty
            }
        } break;
        default: break;
    }
}
