/**
 * @file
 * @author Alice Liang
 *
 * Created on Jan 30, 2019, 5:09 PM
 *
 * @defgroup Run
 * @brief main run file 
 * @{
 */

#ifndef RUN_H
#define RUN_H

/********************************* Includes **********************************/
#include <xc.h>
#include <stdio.h>
#include <stdbool.h>
#include "configBits.h"
//#include "pwmTest.h"
#include "PICArduino.h"
#include "ballDispenser.h"
#include "stack.h"
#include "lcd.h"
#include "logs.h"

/** @brief the main function of the run*/
void run(void);

/** @brief initiates the pins and peripherals of the run*/
void initRun(void);

/** @brief controls the motor*/
void runMotor(int index);

/** @brief turns the robot*/
void turnAround(void);

/** @brief initiates the interrupts*/
void initInterrupt(void);

/**
 * @}
 */

#endif	/* Run_H */
