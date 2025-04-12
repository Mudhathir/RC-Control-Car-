/*
 * macros.h
 *
 *  Created on: Sep 17, 2024
 *      Author: fahimsharifkhaja
 */

#ifndef MACROS_H_
#define MACROS_H_

#define ALWAYS                  (1)
#define RESET_STATE             (0)
#define RED_LED              (0x01) // RED LED 0
#define GRN_LED              (0x40) // GREEN LED 1
#define TEST_PROBE           (0x01) // 0 TEST PROBE
#define TRUE                 (0x01) //



volatile char slow_input_down;
extern char display_line[4][11];
extern char *display[4];
unsigned char display_mode;
extern volatile unsigned char display_changed;
extern volatile unsigned char update_display;
extern volatile unsigned int update_display_count;
extern volatile unsigned int Time_Sequence;
extern volatile char one_time;
unsigned int test_value;
char chosen_direction;
char change;
unsigned int wheel_move;
char forward;
volatile unsigned int Last_Time_Sequence;
volatile unsigned int cycle_time;
volatile unsigned int time_change;
char event;
int segment_count;
int right_motor_count;
int left_motor_count;
//int cycle_time;
int delay_start;








#endif /* MACROS_H_ */
