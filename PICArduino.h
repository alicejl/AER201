/**
 * @file
 * @author Alice Liang
 *
 * Created on Jan 30, 2019, 5:09 PM
 *
 * @defgroup PICArduino
 * @brief Uses I2C to handshake between the PIC and Arduino
 * 
 * Preconditions:
 * @pre pin A5 on Arduino connected to RC4, and A4 to RC4
 * @pre pins are not connected while arduino is programming
 */

#ifndef PICARDUINO_H
#define PICARDUINO_H

/********************************* Includes **********************************/
#include <xc.h>
#include <stdio.h>
#include <stdbool.h>
#include "I2C.h"
#include "configBits.h"

/**@brief initiate I2C line with arduino*/
void initArduino(void);

/**@brief sends a byte to the arduino*/
void sendToArduino(unsigned data);

/**@brief reads a byte from the arduino*/
unsigned int readFromArduino(void);

/**@brief sends an ID to arduino and reads the response. Returns an int*/
unsigned int requestArduino(unsigned id);
/**
 * @}
 */

#endif	/* PICArduino_H */
