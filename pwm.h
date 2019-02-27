/* 
 * File:   pwm.h
 * Author: Alice
 *
 * Created on February 13, 2019, 5:41 PM
 * 
 * @defgroup PWM
 * 
 * @brief PIC Produces a 3.1kHz signal on the pins RC1 and RC2
 * @brief the pins have the same frequency but different duty cycles
 */

#ifndef PWM_H
#define	PWM_H

/********************************* Includes **********************************/
#include <xc.h>

/**
 * @brief Sets the duty cycle, duty 1 corresponds to the RC2 pin and duty 2 to the RC1 pin
 * @param duty The desired duty cycle as a percentage of the period (min: 0, 
 *        max: 100)
 */
void set_pwm_duty_cycle(float duty1,float duty2);

/** @brief Initiates a 3100Hz pwm on pins RC1 and RC2*/
void initPWM(void);


#endif	/* PWM_H */

