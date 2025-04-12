/*
 * ssidip.h
 *
 * Created on: Nov 19, 2024
 *      Author: fahimsharifkhaja
 */

#ifndef SSIDIP_H_
#define SSIDIP_H_


#include "msp430.h"

// Function Prototypes
void ssidIP(void); // Main function to handle SSID and IP operations

// Global Variables (if needed for sharing)
extern unsigned int ssid_detect_flag;
extern unsigned int ssid_index;
char ssid_buffer[11];

extern unsigned int wifi_connect_flag;
// Declare shared functions
void ssidIP(void);

// Declare shared state variables
extern unsigned int iot_boot_timer;
extern char iotState;

// Declare constants
#define WAITIP 1
#define WAITSSID 2
#define IP 3
#define SSID 4
#define SERVER 5
#define MUX 6
#define STORE 7

#endif /* SSIDIP_H_ */
