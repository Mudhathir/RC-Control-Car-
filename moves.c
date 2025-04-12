#include "msp430.h"
#include <string.h>
#include "LCD.h"
#include "ADC.h"
#include "macros.h"
#include "pwm.h"
#include "moves.h"

// Define wheel speed constants for smoother movement
#define RWHEELSL 7000  // Slower speed for right wheel
#define LWHEELSL 7000  // Slower speed for left wheel
#define WHEEL_OFF 0    // Speed for stopping the wheels
volatile unsigned int stop = 0;  // Timer counter variable for timing
int s7 = 0;  // Counter for project-specific timing

// Define motor control pins and ports (adjust these based on your MSP430 setup)
#define RIGHT_MOTOR_FORWARD BIT0  // Pin for right motor forward
#define LEFT_MOTOR_FORWARD BIT1   // Pin for left motor forward
#define MOTOR_PORT P1OUT          // Port for motor control

extern char display_line[4][11];  // Display line array
extern volatile unsigned char display_changed;  // Display change flag
extern volatile unsigned int stop;  // Timer counter variable for timing
extern int s7;
unsigned int p7 = 0;  // State variable for Project 7
unsigned int circle_count = 0;  // Variable to track completed circles
unsigned char blackline
unsigned int black
// Motor control function prototypes
void Wheels_OFF(void);
void Forward_ON(void);
void Turn_Left(void);
void Turn_Right(void);
void AdjustLeft(void);
void AdjustRight(void);
void SpinCCW(void);  // Function for spinning counterclockwise

#define WHITE_THRESHOLD_RIGHT 500
#define WHITE_THRESHOLD_LEFT 500
#define BLACK_THRESHOLD_RIGHT 900
#define BLACK_THRESHOLD_LEFT 900

// Timer threshold for tracking time during circle movement and stopping
#define CIRCLE_TIME_THRESHOLD 1000  // Example threshold for one circle
#define STOP_TIME_THRESHOLD 900     // Timer count for 100 seconds at 0.2-second interval
#define FORWARD_TO_CENTER_TIME 300   // Timer count for moving forward to the center (2 seconds at 0.2-second intervals)

void project_7(void) {
    switch (p7) {
        case 0:  // Move forward until a black line is detected
            strcpy(display_line[2], "FWD SEARCH");
            display_changed = TRUE;
            Forward_ON();
            if (ADC_Right_Det >= BLACK_THRESHOLD_RIGHT || ADC_Left_Det >= BLACK_THRESHOLD_LEFT) {
                Wheels_OFF();  // Stop when either sensor detects black
                stop = 0;  // Reset timer count
                p7 = 1;  // Transition to turning to align with the line
            }
            break;

        case 1:  // Turn until both sensors detect the black line
            strcpy(display_line[2], "ALIGNING");
            display_changed = TRUE;

            if (ADC_Right_Det > 700 && ADC_Left_Det > 700) {
                Turn_Right();  // Turn towards the black line
            } else {
                Wheels_OFF();  // Stop turning when aligned with the line
                p7 = 2;  // Transition to follow the line
                stop = 0;
            }
            break;

        case 2:  // Follow the black line and adjust if off-center
            strcpy(display_line[2], "FOLLOWING");
            display_changed = TRUE;

            if (ADC_Right_Det >= BLACK_THRESHOLD_RIGHT && ADC_Left_Det >= BLACK_THRESHOLD_LEFT) {
                Forward_ON();  // Move forward if centered on the black line

                // Check if the timer count has reached the threshold to stop after 100 seconds
                if (s7 >= STOP_TIME_THRESHOLD) {
                    Wheels_OFF();  // Stop after reaching the time threshold
                    stop = 0;  // Reset timer for center approach
                    p7 = 3;  // Transition to turn toward the center
                }
            } else if (ADC_Left_Det < BLACK_THRESHOLD_LEFT) {
                AdjustRight();  // Adjust right if left sensor is on white
                strcpy(display_line[3], "ADJ RIGHT");
            } else if (ADC_Right_Det < BLACK_THRESHOLD_RIGHT) {
                AdjustLeft();  // Adjust left if right sensor is on white
                strcpy(display_line[3], "ADJ LEFT");
            }
            break;

        case 3:  // Turn counterclockwise until both sensors detect white
            strcpy(display_line[2], "TURNING CCW");
            display_changed = TRUE;
            SpinCCW();  // Spin counterclockwise

            if (stop >= 200) {
                Wheels_OFF();  // Stop spinning when both sensors detect white
                stop = 0;  // Reset timer for moving forward
                p7 = 4;  // Transition to move forward to the center
            }
            break;

        case 4:  // Move forward to the center for 2 seconds
            strcpy(display_line[2], "MOVING TO CTR");
            display_changed = TRUE;
            Forward_ON();  // Move forward

            if (stop >= FORWARD_TO_CENTER_TIME) {  // Stop after moving forward for 2 seconds
                Wheels_OFF();  // Stop the vehicle
                strcpy(display_line[3], "CENTER STOP");
                display_changed = TRUE;
                p7 = 5;  // Reset to initial state or move to an idle state
            }
            break;
        case 5:
            Wheels_OFF();  // Stop in any unexpected state
            break;
        default:
            break;
    }
}

// Motor control function implementations
void end_case(void){
 Forward_OFF();
 state = WAIT;
 event = NONE;
 segment_count = 0;
}
void Turn(void){
    RIGHT_FORWARD_SPEED = RWHEELSL; // P6.1 Right Forward PWM ON amount
    LEFT_FORWARD_SPEED = WHEEL_OFF; // P6.2 Left Forward PWM ON amount
    RIGHT_REVERSE_SPEED = WHEEL_OFF; // P6.3 Right Reverse PWM OFF
    LEFT_REVERSE_SPEED = LWHEELSL; // P6.4 Left Reverse PWM OFF
}

void Forward_OFF(void){
//    P6OUT &= R_FORWARD;
//    P6OUT &= L_FORWARD;
    RIGHT_FORWARD_SPEED = WHEEL_OFF; // P6.1 Right Forward PWM ON amount
    LEFT_FORWARD_SPEED = WHEEL_OFF; // P6.2 Left Forward PWM ON amount
    RIGHT_REVERSE_SPEED = WHEEL_OFF; // P6.3 Right Reverse PWM OFF
    LEFT_REVERSE_SPEED = WHEEL_OFF; // P6.4 Left Reverse PWM OFF
}

void Forward_ON(void){
//    P6OUT |= R_FORWARD;
//    P6OUT |= L_FORWARD;
    RIGHT_FORWARD_SPEED = RWHEELSL; // P6.1 Right Forward PWM ON amount
    LEFT_FORWARD_SPEED = LWHEELSL; // P6.2 Left Forward PWM ON amount
    RIGHT_REVERSE_SPEED = WHEEL_OFF; // P6.3 Right Reverse PWM OFF
    LEFT_REVERSE_SPEED = WHEEL_OFF; // P6.4 Left Reverse PWM OFF
}

void Reverse_On(void) {
//    P6OUT |= R_REVERSE;
//    P6OUT |= L_REVERSE;
//    P6OUT &= ~R_FORWARD;
//    P6OUT &= ~L_FORWARD;

    RIGHT_FORWARD_SPEED = WHEEL_OFF; // P6.1 Right Forward PWM ON amount
    LEFT_FORWARD_SPEED = WHEEL_OFF; // P6.2 Left Forward PWM ON amount
    RIGHT_REVERSE_SPEED = RWHEELSL; // P6.3 Right Reverse PWM OFF
    LEFT_REVERSE_SPEED = LWHEELSL; // P6.4 Left Reverse PWM OFF
}

void SpinCW(void) {
//    P6OUT |= R_REVERSE;
//    P6OUT |= L_FORWARD;
//    P6OUT &= ~R_FORWARD;
//    P6OUT &= ~L_REVERSE;
    RIGHT_FORWARD_SPEED = RWHEELSL; // P6.1 Right Forward PWM ON amount
    LEFT_FORWARD_SPEED = WHEEL_OFF; // P6.2 Left Forward PWM ON amount
    RIGHT_REVERSE_SPEED = WHEEL_OFF; // P6.3 Right Reverse PWM OFF
    LEFT_REVERSE_SPEED = LWHEELSL; // P6.4 Left Reverse PWM OFF

}

void SpinCCW(void) {
    RIGHT_FORWARD_SPEED = RWHEELSL;  // Right wheel moves forward
    LEFT_FORWARD_SPEED = WHEEL_OFF;  // Left wheel stops
    RIGHT_REVERSE_SPEED = WHEEL_OFF;
    LEFT_REVERSE_SPEED = RWHEELSL;  // Left wheel moves backward
}

void Turn_Right(void) {
    RIGHT_FORWARD_SPEED = RWHEELSL / 2;  // Right wheel slows down
    LEFT_FORWARD_SPEED = RWHEELSL;  // Left wheel moves at normal speed
    RIGHT_REVERSE_SPEED = WHEEL_OFF;
    LEFT_REVERSE_SPEED = WHEEL_OFF;
}


void Wheels_OFF(void){
//    P6OUT &= ~R_FORWARD;
//    P6OUT &= ~L_FORWARD;
//    P6OUT &= ~R_REVERSE;
//    P6OUT &= ~L_REVERSE;

    RIGHT_FORWARD_SPEED = WHEEL_OFF; // P6.1 Right Forward PWM ON amount
    LEFT_FORWARD_SPEED = WHEEL_OFF; // P6.2 Left Forward PWM ON amount
    RIGHT_REVERSE_SPEED = WHEEL_OFF; // P6.3 Right Reverse PWM OFF
    LEFT_REVERSE_SPEED = WHEEL_OFF; // P6.4 Left Reverse PWM OFF
}

void AdjustLeft(void) {
    RIGHT_FORWARD_SPEED = RWHEELSL;  // Right wheel moves at normal speed
    LEFT_FORWARD_SPEED = RWHEELSL / 2;  // Left wheel slows down
    RIGHT_REVERSE_SPEED = WHEEL_OFF;
    LEFT_REVERSE_SPEED = WHEEL_OFF;
}

void AdjustRight(void) {
    RIGHT_FORWARD_SPEED = RWHEELSL / 2;  // Right wheel slows down
    LEFT_FORWARD_SPEED = RWHEELSL;  // Left wheel moves at normal speed
    RIGHT_REVERSE_SPEED = WHEEL_OFF;
    LEFT_REVERSE_SPEED = WHEEL_OFF;
}

