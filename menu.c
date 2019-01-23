//main.c
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

const char keys[] = "123A456B789C*0#D"; 

void main(void){
	// RD2 is the character LCD RS
	// RD3 is the character LCD enable (E)
	// RD4-RD7 are character LCD data lines
    	LATD = 0x00;
    	TRISD = 0x00;
    
    	// Set all A/D ports to digital (pg. 222)
    	ADCON1 = 0b00001111;
   
    	// Initialize LCD
	initLCD();
	initMenu();
	
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
                //break;
            }
		
            else if (temp == '2'){
                showLog();
                //break;
            }
    }
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//lcd.c
/**
 * @file
 * @author Michael Ding
 * @author Tyler Gamvrelis
 * @author Alice Liang
 *
 * Created on July 18, 2016, 12:11 PM
 * @ingroup CharacterLCD
 */

/********************************* Includes **********************************/
#include "lcd.h"

/******************************** Constants **********************************/
const unsigned char LCD_SIZE_HORZ = 16;
const unsigned char LCD_SIZE_VERT = 4;

const unsigned char LCD_LINE1_ADDR = 0x0;
const unsigned char LCD_LINE2_ADDR = 0x40;
const unsigned char LCD_LINE3_ADDR = 0x10;
const unsigned char LCD_LINE4_ADDR = 0x50;

/***************************** Private Functions *****************************/
/**
 * @brief Pulses the LCD register enable signal, which causes the LCD to latch
 *        the data on LATD. Interrupts are disabled during this pulse to
 *        guarantee that the timing requirements of the LCD's protocol are met
 */

static inline void pulse_e(void){
    unsigned char interruptState = INTCONbits.GIE;
    di();
    E = 1;
    // This first delay only needs to be 1 microsecond in theory, but 25 was
    // selected experimentally to be safe
    __delay_us(25);
    E = 0;
    __delay_us(100);
    INTCONbits.GIE = interruptState;
}

/**
 * @brief Low-level function to send 4 bits to the display
 * @param data The byte whose 4 least-significant bits are to be sent to the LCD
 */
static void send_nibble(unsigned char data){
    // Send the 4 least-significant bits
    LATD = (unsigned char)(LATD & 0x0F); // Clear LATD[7:4]
    LATD = (unsigned char)((data << 4) | LATD); // Write data[3:0] to LATD[7:4]
    pulse_e();
}

/**
 * @brief Low-level function to send a byte to the display
 * @param data The byte to be sent
 */
static void send_byte(unsigned char data){
    // Send the 4 most-significant bits
    send_nibble(data >> 4);
    
    // Send the 4 least-significant bits
    send_nibble(data);
}

/***************************** Public Functions ******************************/
void lcdInst(char data){
    RS = 0;
    send_byte(data);
}

void initLCD(void){
    __delay_ms(15);
    
    RS = 0;
    // Set interface length to 4 bits wide
    send_nibble(0b0011); 
    __delay_ms(5);
    send_nibble(0b0011);
    __delay_us(150);
    send_byte(0b00110010);
    
    send_byte(0b00101000); // Set N = number of lines (1 or 2) and F = font
    send_byte(0b00001000); // Display off
    send_byte(0b00000001); // Display clear
    __delay_ms(5);
    send_byte(0b00000110); // Entry mode set
    
    // Enforce on: display, cursor, and cursor blinking
    lcd_display_control(true, false, false);
}

void initMenu(void){
	char welcome[] = "    Welcome!   ";
	char inst1[] = "Press number to ";
    char inst2[] = "     select     ";
	char menu1[] = "1. Start        ";
    char menu2[] = "2. Logs         ";

	dispInput(welcome,16);
	__delay_us(1000000);
	send_byte(0b00000001); // Display clear
	dispInput(inst1,16);
    dispInput(inst2,16);
	dispInput(menu1,16);
    dispInput(menu1,16);
}	

void lcd_shift_cursor(unsigned char numChars, lcd_direction_e direction){
    for(unsigned char n = numChars; n > 0; n--){
        lcdInst((unsigned char)(0x10 | (direction << 2)));
    }
}

void lcd_shift_display(unsigned char numChars, lcd_direction_e direction){
    for(unsigned char n = numChars; n > 0; n--){
        lcdInst((unsigned char)(0x18 | (direction << 2)));
    }
}

void putch(char data){
    RS = 1;
    send_byte((unsigned char)data);
}

void dispInput(char *text, int n){ 
	int i=0;
	for (i=0; i<n; i++){
		putch(text[i]);
	}
    return;
}

void run(void){ //temp stand in for running autonomous routine
	char startMessage[] = "Starting run";
    send_byte(0b00001000); // Display off
	send_byte(0b00000001); // Display clear
	dispInput(startMessage, 12);
}

void showLog(void){ //stand in for display run logs 
	char Logs[] = "Showing Logs";
    send_byte(0b00001000); // Display off
	send_byte(0b00000001); // Display clear
	dispInput(Logs, 12);
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              //lcd.h                                                                                                   
/**
 * @file
 * @author Michael Ding
 * @author Tyler Gamvrelis
 * @author Alice Liang
 *
 * Created on August 12, 2016, 4:24 PM
 *
 * @defgroup CharacterLCD
 * @brief Driver for Hitachi HD44780-based character LCD
 * @{
 */

#ifndef LCD_H
#define LCD_H

/********************************* Includes **********************************/
#include <xc.h>
#include <stdio.h>
#include <stdbool.h>
#include "configBits.h"

/********************************** Macros ***********************************/
#define RS LATDbits.LATD2          
#define E  LATDbits.LATD3

/** @brief Clears both LCD lines */
#define lcd_clear(){\
	lcdInst(0x01);\
	__delay_ms(5);\
}

/** @brief Sets cursor position to start of first line */
#define lcd_home(){\
	lcdInst(0x80);\
	__delay_ms(2);\
}

/**
 * @brief Sets the cursor's position to a specific display data RAM (DDRAM)
 *        address
 * @param addr The DDRAM address to move the cursor to (min: 0, max: 127)
 * @note The cursor will not be visible at all addresses
 */
#define lcd_set_ddram_addr(addr){\
    lcdInst(0x80 | addr);\
}

/**
 * @brief Backlight and cursor control
 * @param display_on Turns on the backlight if true, otherwise turns it off
 * @param cursor_on Turns on cursor if true, otherwise turns it off
 * @param blink_cursor Blinks the cursor if true, otherwise cursor is static
 */
#define lcd_display_control(\
	display_on,\
	cursor_on,\
	blink_cursor\
)\
{\
	lcdInst(\
        (unsigned char)(8 | (display_on << 2) | (cursor_on << 1) | blink_cursor)\
    );\
}

/******************************** Constants **********************************/
// Display dimensions as seen in the real world (before you use these in your
// code, double-check that they match the size of your LCD)
extern const unsigned char LCD_SIZE_HORZ; /**< Number of visible columns */
extern const unsigned char LCD_SIZE_VERT; /**< Number of visible rows    */

extern const unsigned char LCD_LINE1_ADDR; /**< Address of first line */
extern const unsigned char LCD_LINE2_ADDR; /**< Address of 2nd line   */
extern const unsigned char LCD_LINE3_ADDR; /**< Address of 3rd line   */
extern const unsigned char LCD_LINE4_ADDR; /**< Address of 4th line   */

/********************************** Types ************************************/
/** @brief The directions the display contents and cursor can be shifted */
typedef enum{
	LCD_SHIFT_LEFT = 0, /**< Left shift  */
	LCD_SHIFT_RIGHT = 1 /**< Right shift */
}lcd_direction_e;

/************************ Public Function Prototypes *************************/
/**
 * @brief Sends a command to a display control register
 * @param data The command byte for the Hitachi controller
 */
void lcdInst(char data);

/** @brief Performs the initial setup of the LCD */
void initLCD(void);

/**
 * @brief Moves the cursor in a given direction by numChars characters
 * @param numChars The number of character positions by which the cursor is to
 *        be moved (min: 0, max: 127)
 * @param direction The direction for which the shift is to occur
 */
void lcd_shift_cursor(unsigned char numChars, lcd_direction_e direction);

/**
 * @brief Shifts the display in a given direction by numChars characters
 * @param numChars The number of character positions by which the display
 *        contents are to be shifted (min: 0, max: 127)
 * @param direction The direction for which the shift is to occur
 */
void lcd_shift_display(unsigned char numChars, lcd_direction_e direction);

/**
 * @brief Sends a character to the display for printing
 * @details The familiar C function printf internally calls a function named
 *          "putch" (put character) whenever a character is to be written to
 *          the screen. Here we have chosen to implement putch so that it
 *          sends the character to the LCD, but you can choose to implement it
 *          however you'd like (e.g. send the character over UART, etc.)
 * @param data The character (byte) to be displayed
 */
void putch(char data);

/**
 * @brief initiate and display welcome and preset menu
 */
void initMenu(void);

/**
 * @brief displays strings of text on lcd
 * @param char string and int length of string
 */
void dispInput(char *text, int n);

void run(void);
void showLog(void);

/**
 * @}
 */

#endif	/* LCD_H */                                                                                                                                                         