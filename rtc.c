/**
 * @file
 * @author Alice Liang
 * 
 * Created on January 18, 2019, 4:07 PM
 * 
 * @defgroup RTC
 * @brief Reads and displays the date time
 *        
 * 
 * Preconditions:
 * @pre Jumpers JP6, and JP7 in the RTC module are shorted
 * @pre RTC chip is properly situated in its socket
 * @pre CR2032 20 mm coin battery is properly situated in its socket and is not
 *      low on charge
 */

/********************************* Includes **********************************/
#include "rtc.h"

/******************************** Constants **********************************/

const char keys[] = "123A456B789C*0#D";


void dateTime(void){
    //char date;
    lcd_clear();
    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    printf("  Date & Time");
    lcd_set_ddram_addr(LCD_LINE4_ADDR);
    printf(" Press * to end");

    I2C_Master_Init(100000);
    
    unsigned char time[7]; // Create a byte array to hold time read from RTC

    //while (1){
        // Reset RTC memory pointer
        I2C_Master_Start(); // Start condition
        I2C_Master_Write(0b11010000); // 7 bit RTC address + Write
        I2C_Master_Write(0x00); // Set memory pointer to seconds
        I2C_Master_Stop(); // Stop condition

        // Read current time
        I2C_Master_Start(); // Start condition
        I2C_Master_Write(0b11010001); // 7 bit RTC address + Read
        for(unsigned char i = 0; i < 6; i++){
            time[i] = I2C_Master_Read(ACK); // Read with ACK to continue reading
        }
        time[6] = I2C_Master_Read(NACK); // Final Read with NACK
        I2C_Master_Stop(); // Stop condition
        
        // Print received data on LCD
        lcd_set_ddram_addr(LCD_LINE2_ADDR);
        printf("Date: %02x/%02x/20%02x", time[4],time[5],time[6]); // Print date in YY/MM/DD
        lcd_set_ddram_addr(LCD_LINE3_ADDR);
        printf("Time: %02x:%02x:%02x", time[2],time[1],time[0]); // HH:MM:SS

            while(PORTBbits.RB1 == 0){continue;}
            unsigned char keypress = (PORTB & 0xF0) >>4;
            while(PORTBbits.RB1 == 1){continue;}
        
            unsigned char temp = keys[keypress];
            if (temp == '*'){
                return;
            }
        __delay_ms(1000);
    //}
}

