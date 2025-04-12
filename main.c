#include "msp430.h"
#include <string.h>
#include "functions.h"
#include "LCD.h"
#include "ports.h"
#include "macros.h"
#include "moves.h"
#include "switches.h"
#include "timersB0.h"
#include "hex2bdc.h"
#include "ADC.h"
#include "hardware.h"
#include "ssidip.h"

#define ALWAYS (1)
#define RESET_STATE (0)
#define RED_LED (0x01) // RED LED 0
#define GRN_LED (0x40) // GREEN LED 1
#define TEST_PROBE (0x01) // 0 TEST PROBE
#define TRUE (0x01) //

// Function Prototypes
void main(void);
void Init_Conditions(void);
void Display_Process(void);
void Init_LEDs(void);
void Init_Serial_UCA1(void);
void Init_Serial_UCA0(void);
void Change_Baud_Rate(unsigned int baud_rate);
void delay_ms(unsigned int ms);

// Global Variables
volatile unsigned int sw1_position = 0;
volatile unsigned int sw2_position = 0;
unsigned int readp = 0;
unsigned int transmit = 0;
char PBuff[11] = {0}; // Properly sized buffer

unsigned int iot_time;
// Declare external variables defined in hardware.c
extern unsigned int iot_rx_wr;
extern char IOT_Ring_Rx[11];
extern char iot_TX_buf[11];
extern char display_line[4][11];
extern volatile unsigned char display_changed;

void main(void) {
    int i = 0; // Loop variable

    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    // Initial Setup
    PM5CTL0 &= ~LOCKLPM5; // Disable high-impedance mode
    Init_Ports();
    Init_Clocks();
    Init_Conditions();
    Init_Timers();
    Init_LCD();
    Init_ADC();
    Init_Serial_UCA1();
    Init_Serial_UCA0();

    // Initial display message
//    strcpy(display_line[0], "  Waiting ");
//    strcpy(display_line[2], "115,200"); // Display initial baud rate
    display_changed = TRUE;
    Display_Process();

    // Main loop
    while (ALWAYS) {
        ssidIP();
        movement_machine();
        iot_process_commands();
        project_7();

        // Check for new data received from AD3
        if(iot_time >= 15){
            P3OUT |= IOT_EN;
            P3OUT |= IOT_LINK_GRN;

        }
//        if (readp != iot_rx_wr) {
//            char received_char = IOT_Ring_Rx[readp++];
//            if (received_char == '\n') {
//                PBuff[i] = '\0'; // Null-terminate received string
//                strcpy(display_line[0], " Received ");
//                strcpy(display_line[3], PBuff); // Display received data on line 4
//                display_changed = TRUE;
//                Display_Process();
//
//                // Prepare for transmission
//                strcpy(iot_TX_buf, PBuff); // Copy received data to the transmission buffer
//                transmit = 1;
//
//                memset(PBuff, ' ', sizeof(PBuff)); // Clear PBuff after use
//                i = 0; // Reset buffer index
//
//                // Enable transmission
//                UCA0IE |= UCTXIE;
//
//            } else {
//                if (i < sizeof(PBuff) - 1) { // Ensure no overflow
//                    PBuff[i++] = received_char; // Store character in buffer
//                }
//            }
//
//            if (readp >= sizeof(IOT_Ring_Rx)) {
//                readp = 0;
//            }
//        }
//
//        // Handle transmission state
//        if (transmit == 1) {
//            transmit = 0;
//            strcpy(display_line[0], " Transmit ");
//            strcpy(display_line[1], iot_TX_buf); // Display command on line 2
//            UCA0IE |= UCTXIE;
//            display_changed = TRUE;
//            Display_Process();
//        }

        // Handle baud rate switching (using switches as input)
//        if (sw1_position == 1) {
//            sw1_position = 0; // Reset switch position
//            Change_Baud_Rate('a'); // Change to 115200
//            strcpy(display_line[2], "115,200");
//            display_changed = TRUE;
//            Display_Process();
//        }

//        if (sw2_position == 1) {
//            sw2_position = 0; // Reset switch position
//            Change_Baud_Rate('b'); // Change to 460800
//            strcpy(display_line[2], "460,800");
//            display_changed = TRUE;
//            Display_Process();
//        }
    }
}

void Change_Baud_Rate(unsigned int baud_rate) {
    if (baud_rate == 'a') {
        UCA1BRW = 4;
        UCA1MCTLW = 0x5551;
        UCA0BRW = 4;
        UCA0MCTLW = 0x5551;
    } else if (baud_rate == 'b') {
        UCA1BRW = 17;
        UCA1MCTLW = 0x4A00;
        UCA0BRW = 17;
        UCA0MCTLW = 0x4A00;
    }
    UCA1CTLW0 &= ~UCSWRST; // Release from reset
    UCA0CTLW0 &= ~UCSWRST;
}
void delay_ms(unsigned int ms) {
    while (ms--) {
        __delay_cycles(1000); // Assuming 1 MHz clock; adjust as needed for different clock speeds
    }
}

