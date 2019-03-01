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
    b=readFromArduino();//reads 8 least significant bits
    a=readFromArduino();//reads 8 most significant bits
    unsigned int data = a;
    data=(data<<8)|b;//combines int
    return data;
}
