/**
 * @file
 * @author Alice Liang
 * @author Michael Ding
 * @author Tyler Gamvrelis
 * 
 * Created on July 18, 2016, 12:11 PM
 * 
 * @defgroup RTC
 * @brief Sets the real-time clock (RTC) 
 * 
 * Preconditions:
 * @pre Jumpers JP6, and JP7 in the RTC module are shorted
 * @pre RTC chip is properly situated in its socket
 * @pre CR2032 20 mm coin battery is properly situated in its socket and is not
 *      low on charge
 */

#include <xc.h>
#include <stdio.h>
#include <configBits.h>
#include "I2C.h"

const char happynewyear[7] = {
    0x45, // 45 Seconds 
    0x59, // 59 Minutes
    0x15, // 24 hour mode, set to 23:00
    0x03, // Wednesday
    0x30, // 30st
    0x01, // January
    0x19  // 201
};

void main(void) {
    // RD2 is the character LCD RS
    // RD3 is the character LCD enable (E)
    // RD4-RD7 are character LCD data lines
    LATD = 0x00;
    TRISD = 0x00;
    initLCD();

    // Initialize I2C Master with 100 kHz clock
    I2C_Master_Init(100000);
    
    // Set the time in the RTC. To see the RTC keep time, comment this line out
    // after programming the PIC directly before with this line included
    rtc_set_time();
}


/** @brief Writes the happynewyear array to the RTC memory */
void rtc_set_time(void){
    I2C_Master_Start(); // Start condition
    I2C_Master_Write(0b11010000); //7 bit RTC address + Write
    I2C_Master_Write(0x00); // Set memory pointer to seconds
    
    // Write array
    for(char i=0; i < 7; i++){
        I2C_Master_Write(happynewyear[i]);
    }
    
    I2C_Master_Stop(); //Stop condition
}