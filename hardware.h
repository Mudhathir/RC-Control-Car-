/*
 * hardware.h
 *
 *  Created on: Nov 1, 2024
 *      Author: fahimsharifkhaja
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

#define BEGINNING (0)
extern volatile unsigned int uart_message_ready;
extern char uart_rx_buffer[11];
//void Init_Serial_UCA0(char speed);
//void Init_Serial_UCA1(char speed);

// Declare shared variables
extern char IOT_Ring_Rx[11];
extern char iot_TX_buf[11];
extern unsigned int iot_rx_wr;
extern unsigned int iot_tx;




#endif /* HARDWARE_H_ */
