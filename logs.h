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

/**@brief writes integers to the EEPROM*/
void EEPROM_WriteInt(unsigned char address,unsigned int val);

/**@brief writes individual bytes to the EEPROM*/
void EEPROM_WriteByte(unsigned char eepromAddress, unsigned char eepromData);

/**@brief extracts all the data about a run from the EEPROM, returns via pointers*/
void readLog(int index, unsigned int* can, unsigned int* time, unsigned int* fullCan, unsigned int* emptCan, unsigned int* ball);

/**@brief extracts and returns an integer from EEPROM*/
unsigned int EEPROM_ReadInt(unsigned char address);

/**@brief extracts and returns a byte from EEPROM*/
unsigned char EEPROM_ReadByte(unsigned char eepromAddress);

/**@brief prints formatted logs to the lcd. 
 * @param index starts at 0 and has a max of 3*/
void dispLog(char index);



#endif	/* LOGS_H */

