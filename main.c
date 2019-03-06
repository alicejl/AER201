/**
 * @file
 * @author Alice Liang
 * 
 * Created on January 18, 2019, 4:07 PM
 * 
 * @defgroup KeypadLCD_IO_Polling
 * @brief Receives keypress data from co-processor and displays the
 *        corresponding characters on the character LCD
 * 
 * Preconditions:
 * @pre Co-processor running default program
 * @pre Character LCD in PIC socket
 */

#include <xc.h>
#include "configBits.h"
#include "lcd.h"
#include "run.h"

const char keys[] = "123A456B789C*0#D"; 

void menuLoop(void);

void main(void){
	// RD2 is the character LCD RS
	// RD3 is the character LCD enable (E)
	// RD4-RD7 are character LCD data lines
    	LATD = 0x00;
    	TRISD = 0x00;
    
    	// Set all A/D ports to digital (pg. 222)
    	ADCON1 = 0b00001111;
   
    	initLCD();// Initialize LCD
        lcd_set_ddram_addr(LCD_LINE1_ADDR);
        printf("    Welcome!");
        __delay_us(2500000);
	while(1){
        	initMenu();
        	menuLoop();
	}
}

void menuLoop(void){
	// Main loop
	while(1){
        	// RB1 is the interrupt pin, so if there is no key pressed, RB1 will be
	        // 0. The PIC will wait and do nothing until a key press is signaled
        	while(PORTBbits.RB1 == 0){  continue;   }
        
        	// Read the 4-bit character code
	        unsigned char keypress = (PORTB & 0xF0) >> 4;
        
        	// Wait until the key has been released
	        while(PORTBbits.RB1 == 1){  continue;   }
        
        	Nop(); // Apply breakpoint here to prevent compiler optimizations
        
        	unsigned char temp = keys[keypress];
	        if (temp == '1'){ 
                run();
                return;
            }
		
            else if (temp == '2'){
                showLog();
                return;
            }
            
            else if (temp == '3'){
                dateTime();
                return;
            }
            else{
                lcd_clear();
                lcd_set_ddram_addr(LCD_LINE1_ADDR);
                printf(" Invalid Input!");
                lcd_set_ddram_addr(LCD_LINE2_ADDR);
                printf("Select 1,2 or 3");
                __delay_us(1000000);
                return;
            }
    }
}
