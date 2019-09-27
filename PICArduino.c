/**
 * @file
 * @author Michael Ding
 * @author Tyler Gamvrelis
 * @author Alice Liang
 *
 * Created on Feb 22, 2019, 10:57 AM
 * 
 * @defgroup PIC_I2C_Arduino
 * @brief handshake with arduino nano and exchange sensor data
 * 
 * Preconditions:
 * @pre PIC-Arduino link switches are enabled (ON) for A4 and A5 of the Arduino
 *      Nano (RC4 and RC3 for the primary PIC, respectively)
 * @pre PIC and Arduino Nano agree on the Arduino Nano's slave address (i.e. the
 *      same slave address is used in software)
 * @pre A PC is connected if the Arduino Nano serial monitor is to be used. Note
 *      that the serial monitor baud rate must be 9600 for this program
 */


#include <xc.h>
#include <stdbool.h>
#include "configBits.h"
#include "PICArduino.h"

void initArduino(void){
    I2C_Master_Init(100000); 
    I2C_Master_Start();
    I2C_Master_Write(0b00010000); // 7-bit Arduino slave address + write
    I2C_Master_Stop();
}

void sendToArduino(unsigned data){
    I2C_Master_Start(); // Start condition
    I2C_Master_Write(0b00010000); // 7-bit Arduino slave address + write
    I2C_Master_Write(data); // Write key press data
    I2C_Master_Stop();
}

unsigned int readFromArduino(void){
    I2C_Master_Start();
    I2C_Master_Write(0b00010001); // 7-bit Arduino slave address + Read
    unsigned int data = I2C_Master_Read_Int(NACK); // Read one char only
    I2C_Master_Stop();
    return data;
}

unsigned int requestArduino(unsigned id){
    sendToArduino(id);
    unsigned int a;
    unsigned b;
    b=readFromArduino();
    a=readFromArduino();
    unsigned int data = a;
    data=(data<<8)|b;
    return data;
}
