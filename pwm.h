/* 
 * File:   pwm.h
 * Author: Alice
 *
 * Created on February 13, 2019, 5:41 PM
 */

#ifndef PWM_H
#define	PWM_H

/********************************* Includes **********************************/
#include <xc.h>
#include <math.h>

void set_pwm_duty_cycle(float duty1,float duty2);
void initPWM(void);


#endif	/* PWM_H */

