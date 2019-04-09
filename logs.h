/* 
 * File:   logs.h
 * Author: Alice Liang
 *
 * Created on February 27, 2019, 2:11 AM
 */

#ifndef LOGS_H
#define	LOGS_H

/********************************* Includes **********************************/
#include <xc.h>
#include "configBits.h"
#include <stdbool.h>
#include "lcd.h"

/**@brief store all the data from a run into the EEPROM, handles the addresses*/
void writeLog(unsigned int can, unsigned int time, unsigned int fullCan, unsigned int emptCan, unsigned int ball);

/**@brief writes to EEPROM*/
void EEPROM_Write( unsigned char address,unsigned char data );

/**@brief extracts all the data about a run from the EEPROM, returns via pointers*/
void readLog(int index, unsigned int* can, unsigned int* time, unsigned int* fullCan, unsigned int* emptCan, unsigned int* ball);

/**@brief extracts and returns a value from EEPROM*/
unsigned char EEPROM_Read( unsigned char address );

/**@brief prints formatted logs to the lcd. 
 * @param index starts at 0 and has a max of 3*/
void dispLog(char index);



#endif	/* LOGS_H */

