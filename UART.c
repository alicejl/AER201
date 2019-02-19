/**
 * @file
 * @author Tyler Gamvrelis
 * @author Alice Liang
 * 
 * Created on February 15, 2019, 4:51 PM
 * 
 * @defgroup PIC_UART_Arduino
 * @brief Demonstrates arbitrary information exchange between the main PIC and
 *        the Arduino Nano. To see the results on the Arduino Nano side, use
 *        the Arduino sample program Arduino_UART_PIC
 * 
 * Preconditions:
 * @pre The Co-processor is not driving lines on the UART bus (e.g. the JP_TX
 *      and JP_RX jumpers are removed)
 * @pre The character LCD is in an Arduino Nano socket
 * @pre PIC-Arduino link switches are enabled (ON) for D1 of the Arduino (the RX
 *      pin). However, make sure that neither of D0 and D1 are enabled (ON) 
 *      while programming the Arduino Nano
 */

#include <xc.h>
#include <configBits.h>
#include "UART.h"


void initUART(void){
    // Configure the baud rate generator for 9600 bits per second
    long baudRate = 9600;
    SPBRG = (unsigned char)((_XTAL_FREQ / (64 * baudRate)) - 1);
    
    // Configure transmit control register
    TXSTAbits.TX9 = 0; // Use 8-bit transmission (8 data bits, no parity bit)
    TXSTAbits.SYNC = 0; // Asynchronous communication
    TXSTAbits.TXEN = 1; // Enable transmitter
    __delay_ms(5); // Enabling the transmitter requires a few CPU cycles for stability
    
    // Configure receive control register
    RCSTAbits.RX9 = 0; // Use 8-bit reception (8 data bits, no parity bit)
    RCSTAbits.CREN = 1; // Enable receiver
    
    // Enforce correct pin configuration for relevant TRISCx
    TRISCbits.TRISC6 = 0; // TX = output
    TRISCbits.TRISC7 = 1; // RX = input
   
    // Enable serial peripheral
    RCSTAbits.SPEN = 1;
}

void sendToArduino(unsigned char* msg){
    while(1){
        // Iterate sizeof(msg) - 1 times so that we don't send the null char
        // at the end of the string
        for(unsigned char i = 0; i < sizeof(msg) - 1; i++)
        {
            // Wait until the previous TXREG data has completed its transfer 
            // into the TSR and the TSR has finished transmitting all bits 
            // before loading the TXREG register
            while(!TXIF | !TRMT){
                continue;
            }

            // Load data into the transmit register, TXREG
            TXREG = msg[i];
        }
    }
}