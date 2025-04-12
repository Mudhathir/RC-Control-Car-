#include "msp430.h"
#include <string.h>
#include "functions.h"
#include "LCD.h"
#include "ports.h"
#include "macros.h"
#include "timersB0.h"
#include "hardware.h"
#include "ssidip.h"
#include "moves.h" // Include this for movement-related functions

// Global variables
extern unsigned int iot_time;
extern unsigned int readp;
extern unsigned int iot_rx_wr;
extern char IOT_Ring_Rx[11];
extern char iot_TX_buf[11];
extern char display_line[4][11];
extern volatile unsigned char display_changed;

#define BEGINNING 0

// Execution flags for timed sequences
unsigned char sysstore_executed = FALSE;
unsigned char cipmux_executed = FALSE;
unsigned char cipserver_executed = FALSE;
unsigned char ssid_query_executed = FALSE;
unsigned char ip_query_executed = FALSE;

// Movement command parsing
unsigned int caret_detect_flag = 0;
unsigned char iot_command_buffer[3][10];
unsigned int command_index = 0;
unsigned int command_char_index = 0;

// Movement state and durations
unsigned char current_movement = 0;
unsigned int movement_timer = 0;
unsigned int movement_duration = 0;

// Movement state definitions
#define STATE_FORWARD 1
#define STATE_BACKWARD 2
#define STATE_RIGHT 3
#define STATE_LEFT 4
#define STATE_NONE 0
#define STATE_STOP 5

// State handling for SSID and IP parsing
unsigned char ssid_parsing_state = 0;
unsigned char ip_parsing_state = 0;
unsigned char wifi_connected = FALSE; // Tracks Wi-Fi connection status
char parsed_ssid[11] = { 0 };
char parsed_ip[15] = { 0 };
unsigned int ssid_buffer_index = 0;
unsigned int ip_buffer_index = 0;

// IoT state definitions
#define STATE_SSID 1
#define STATE_IP 2
#define STATE_NONE 0

unsigned char current_state = STATE_NONE;

void ssidIP(void)
{
    // Initialize IoT module and setup commands
    if (iot_time > 50)
    {
        P3OUT |= IOT_EN; // Enable IoT module
    }

    if (iot_time > 500 && !sysstore_executed)
    {
        strcpy(iot_TX_buf, "AT+SYSSTORE=0\r\n");
        UCA0IE |= UCTXIE;
        sysstore_executed = TRUE;
    }

    if (iot_time > 750 && !cipmux_executed)
    {
        strcpy(iot_TX_buf, "AT+CIPMUX=1\r\n");
        UCA0IE |= UCTXIE;
        cipmux_executed = TRUE;
    }

    if (iot_time > 1000 && !cipserver_executed)
    {
        strcpy(iot_TX_buf, "AT+CIPSERVER=1,1111\r\n");
        UCA0IE |= UCTXIE;
        cipserver_executed = TRUE;
    }

    if (iot_time > 1250 && !ssid_query_executed)
    {
        strcpy(iot_TX_buf, "AT+CWJAP?\r\n"); // Query Wi-Fi status
        current_state = STATE_SSID;
        UCA0IE |= UCTXIE;
        ssid_query_executed = TRUE;
    }

    if (current_state == STATE_SSID && iot_time > 1500 && !ip_query_executed)
    {
        strcpy(iot_TX_buf, "AT+CIFSR\r\n"); // Query IP address
        current_state = STATE_IP;
        UCA0IE |= UCTXIE;
        ip_query_executed = TRUE;
    }

    // Parse SSID and IP information
    switch (current_state)
    {
    case STATE_SSID:
        if (readp != iot_rx_wr)
        {
            if (!ssid_parsing_state && IOT_Ring_Rx[readp++] == '"')
            {
                ssid_parsing_state = 1;
            }
            else if (ssid_parsing_state == 1)
            {
                if (IOT_Ring_Rx[readp] != '"')
                {
                    parsed_ssid[ssid_buffer_index++] = IOT_Ring_Rx[readp++];
                }
                else
                {
                    ssid_parsing_state = 2;
                }
            }
        }

        if (readp >= sizeof(IOT_Ring_Rx))
        {
            readp = BEGINNING;
        }

        if (ssid_parsing_state == 2)
        {
            strncpy(display_line[0], parsed_ssid, 10);
            display_changed = TRUE;
            // Check for a successful Wi-Fi connection
            if (strstr(parsed_ssid, "CONNECTED"))
            { // Check if connected
                wifi_connected = TRUE;
            }
        }
        break;

    case STATE_IP:
        if (readp != iot_rx_wr)
        {
            if (!ip_parsing_state && IOT_Ring_Rx[readp++] == '"')
            {
                ip_parsing_state = 1;
            }
            else if (ip_parsing_state == 1)
            {
                if (IOT_Ring_Rx[readp] != '"')
                {
                    parsed_ip[ip_buffer_index++] = IOT_Ring_Rx[readp++];
                }
                else
                {
                    ip_parsing_state = 2;
                }
            }
        }

        if (readp >= sizeof(IOT_Ring_Rx))
        {
            readp = BEGINNING;
        }

        if (ip_parsing_state == 2)
        {
            //  strcpy(display_line[1], "    IP    ");
            strncpy(display_line[1], parsed_ip, 10);
            display_changed = TRUE;
        }
        break;

    default:
        break;
    }

    // Turn on green LED only if connected to Wi-Fi
    if (wifi_connected)
    {
        P3OUT |= IOT_LINK_GRN; // Turn on green LED
    }
    else
    {
        P3OUT &= ~IOT_LINK_GRN; // Turn off green LED
    }

    // Process movement commands
    if (wifi_connected && iot_time > 1750)
    { // Only process if connected to Wi-Fi
        iot_process_commands(); // Process received commands
    }
}

void iot_process_commands(void)
{
    // Parse received commands from the ring buffer
    if (readp != iot_rx_wr)
    {
        if (!caret_detect_flag && IOT_Ring_Rx[readp++] == '^')
        {
            caret_detect_flag = 1;
        }
        else if (caret_detect_flag == 1)
        {
            if (IOT_Ring_Rx[readp] != '\r')
            {
                iot_command_buffer[command_index][command_char_index++] =
                        IOT_Ring_Rx[readp++];
            }
            else
            {
                caret_detect_flag = 2;
            }
        }
    }

    if (caret_detect_flag == 2)
    {
        char *cmd = iot_command_buffer[command_index];

        if (cmd[0] == 'F')
        {
            current_movement = STATE_FORWARD;
            movement_duration = (cmd[1] - '0') * 1000;
        }
        else if (cmd[0] == 'B')
        {
            current_movement = STATE_BACKWARD;
            movement_duration = (cmd[1] - '0') * 1000;
        }
        else if (cmd[0] == 'R')
        {
            current_movement = STATE_RIGHT;
            movement_duration = (cmd[1] - '0') * 500;
        }
        else if (cmd[0] == 'L')
        {
            current_movement = STATE_LEFT;
            movement_duration = (cmd[1] - '0') * 500;
        }
        else if (cmd[0] == 'Q')
        {
            current_movement = STATE_STOP;
            movement_duration = (cmd[1] - '0') * 500;
        }

        else if (cmd[0] == '1')
        {
            strcpy(display_line[0], "ARRIVED 01");
            display_changed = TRUE;
            Display_Update(0, 0, 0, 0);
        }
        else if (cmd[0] == '2')
        {
            strcpy(display_line[0], "ARRIVED 02");
            display_changed = TRUE;
            Display_Update(0, 0, 0, 0);
        }
        else if (cmd[0] == '3')
        {
            strcpy(display_line[0], "ARRIVED 03");
            display_changed = TRUE;
            Display_Update(0, 0, 0, 0);
        }
        else if (cmd[0] == '4')
        {
            strcpy(display_line[0], "ARRIVED 04");
            display_changed = TRUE;
            Display_Update(0, 0, 0, 0);
        }
        else if (cmd[0] == '5')
        {
            strcpy(display_line[0], "ARRIVED 05");
            display_changed = TRUE;
            Display_Update(0, 0, 0, 0);
        }
        else if (cmd[0] == '6')
        {
            strcpy(display_line[0], "ARRIVED 06");
            display_changed = TRUE;
            Display_Update(0, 0, 0, 0);
        }

        else if (cmd[0] == '7')
        {
            strcpy(display_line[0], "ARRIVED 07");
            display_changed = TRUE;
            Display_Update(0, 0, 0, 0);
        }
        command_index = (command_index + 1) % 3;
        command_char_index = 0;
        caret_detect_flag = 0;
    }
    if (readp >= sizeof(IOT_Ring_Rx))
    {
        readp = 0;
    }
}

void movement_machine(void)
{
    switch (current_movement)
    {
    case STATE_FORWARD:
        if (movement_timer < movement_duration)
        {
            Forward_ON();
        }
        else
        {
            Wheels_OFF();
            current_movement = STATE_NONE;
        }
        break;

    case STATE_BACKWARD:
        if (movement_timer < movement_duration)
        {
            Reverse_On();
        }
        else
        {
            Wheels_OFF();
            current_movement = STATE_NONE;
        }
        break;

    case STATE_RIGHT:
        if (movement_timer < movement_duration)
        {
            SpinCW();
        }
        else
        {
            Wheels_OFF();
            current_movement = STATE_NONE;
        }
        break;

    case STATE_LEFT:
        if (movement_timer < movement_duration)
        {
            SpinCCW();
        }
        else
        {
            Wheels_OFF();
            current_movement = STATE_NONE;
        }
        break;

    case STATE_STOP:
        Wheels_OFF();  // Immediately stop motors
        current_movement = STATE_NONE;
        movement_timer = 0;
        movement_duration = 0;
        break;

    default:
        break;
    }
}
