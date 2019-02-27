/* 
 * File:   ballDispense.h
 * Author: Alice
 *
 * Created on February 13, 2019, 5:41 PM
 * 
 * @defgroup Dispensing
 * @brief initiates and controls the position servo
 * @param 50Hz position servo connected to RC5 pin
 */

#ifndef BALLDISPENSER_H
#define	BALLDISPENSER_H

/********************************* Includes **********************************/
#include <xc.h>
#include "configBits.h"

/**@brief initiates the servo pin RC5 and sets it to start position 90*/
void initDispenser(void);

/**@brief set the servo to 0 degrees gens signal for 40 periods*/
void rotate0(void);

/**@brief set the servo to 90 degrees gens signal for 40 periods*/
void rotate90(void);

/**@brief runs the dispense sequence for servo*/
void dropBall(void);


#endif	/* ballDispensor_H */