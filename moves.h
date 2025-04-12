/*
 * moves.h
 *
 *  Created on: Sep 27, 2024
 *      Author: fahimsharifkhaja
 */

#ifndef MOVES_H_
#define MOVES_H_
#define NONE ('N')
#define STRAIGHT ('L')
#define CIRCLE ('C')
#define WAIT ('W')
#define FIGURE ('F')
#define TRIANGLE ('T')
#define START ('S')
#define RUN ('R')
#define END ('E')
#define RUN_2ND ('2')
#define P5 ('P')
#define PROJ6 ('R')
extern volatile unsigned int stop;
extern int s7;

#define WHEEL_COUNT_TIME (20)
#define RIGHT_COUNT_TIME (15)
#define LEFT_COUNT_TIME (20)
#define LEFT_COUNT_TIME_FIGURE (4)
#define RIGHT_COUNT_TIME_FIGURE (16)
#define LEFT_COUNT_TIME_CIRCLE (4)
#define RIGHT_COUNT_TIME_TRIANGLE (10)
#define LEFT_COUNT_TIME_TRIANGLE (10)
#define RIGHT_COUNT_TIME_TRIANGLE_STRAIGHT (4)
#define LEFT_COUNT_TIME_TRIANGLE_STRAIGHT (12)
#define RIGHT_COUNT_TIME_CIRCLE_2 (16)
#define LEFT_COUNT_TIME_CIRCLE_2 (4)

#define RIGHT_COUNT_TIME_TRIANGLE_TURN (6)

#define RIGHT_COUNT_TIME_CIRCLE (16)
#define WAITING2START (200)
#define TRAVEL_DISTANCE_FIGURE (90)
#define TRAVEL_DISTANCE_CIRCLE (50)
#define TRAVEL_DISTANCE (200)
#define TRAVEL_DISTANCE_TRIANGLE (10)
#define TRAVEL_DISTANCE_CIRCLE_2 (80)

//unsigned int Last_Time_Sequence;
//unsigned int cycle_time;
//unsigned int time_change;
//char event;
//char state;
//unsigned int left_motor_count;
//unsigned int right_motor_count;
//unsigned int segment_count;
//unsigned int delay_start;
//unsigned int repeat;
char state;
extern unsigned int time;  // Declare time as unsigned int

char cycle;
char repeat;
char choice;
int not_complete;
int instruction;
unsigned int p6;
extern volatile unsigned int ADC_Left_Det;
extern volatile unsigned int ADC_Right_Det;

void Turn(void);
void Run_Straight(void);
void Run_Circle(void);
void Run_Figure_8(void);
void Run_Triangle(void);

void start_case(void);
void run_straight_case(void);
void run_circle_case(void);
void run_figure_case(void);
void run_triangle_case(void);
void control_motors(void);
void run_figure_2nd(void);
void end_case(void);
void wait_case(void);

void Forward_OFF(void);
void Forward_ON(void);


void Motors_Off(void);
void SpinCCW(void);
void SpinCW(void);
void Reverse_On(void);
void start_case(void);
void wait_case(void);
void Run_5(void);


#endif /* MOVES_H_ */
