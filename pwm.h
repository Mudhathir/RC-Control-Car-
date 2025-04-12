/*
 * pwm.h
 *
 *  Created on: Oct 19, 2024
 *      Author: fahimsharifkhaja
 */

#ifndef PWM_H_
#define PWM_H_

#define PWM_PERIOD (TB3CCR0)

#define LCD_BACKLITE_BRIGHTNESS (TB3CCR1)

#define RIGHT_FORWARD_SPEED     (TB3CCR2)

#define LEFT_FORWARD_SPEED      (TB3CCR3)

#define RIGHT_REVERSE_SPEED     (TB3CCR4)

#define LEFT_REVERSE_SPEED      (TB3CCR5)

#define WHEEL_OFF (0)
#define SLOW (15000)
#define FAST (50000)
#define LEFT_WHEEL_SLOW (15000)
#define RIGHT_WHEEL_SLOW (12000)
#define PERCENT_100 (50000)
#define PERCENT_80 (45000)





#endif /* PWM_H_ */
