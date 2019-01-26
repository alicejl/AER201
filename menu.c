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
        __delay_us(750000);
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
                printf("  Invalid Input!");
                lcd_set_ddram_addr(LCD_LINE2_ADDR);
                printf("Select 1,2 or 3");
                __delay_us(1000000);
                return;
            }
    }
}

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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

const unsigned char LCD_LINE1_ADDR = 0;
const unsigned char LCD_LINE2_ADDR = 0x40;
const unsigned char LCD_LINE3_ADDR = 0x10;
const unsigned char LCD_LINE4_ADDR = 0x50;

const char keys[] = "123A456B789C*0#D"; 

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
    lcd_clear();
    lcd_set_ddram_addr(LCD_LINE1_ADDR);
	printf("Press to select");
    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    printf("1. Start");
    lcd_set_ddram_addr(LCD_LINE3_ADDR);
    printf("2. Logs");
    lcd_set_ddram_addr(LCD_LINE4_ADDR);
    printf("3. Date and Time");
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

void run(void){ //temp stand in for running autonomous routine
	lcd_clear();
    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    printf("  Starting Run");
    lcd_set_ddram_addr(LCD_LINE3_ADDR);
    printf(" Press * to end");
    while (1){
        while(PORTBbits.RB1 == 0){ continue;}
        unsigned char keypress = (PORTB & 0xF0) >>4;
        while(PORTBbits.RB1 == 1){continue;}
        
        unsigned char temp = keys[keypress];
        if (temp == '*'){
            return;
        }
    }
}

void showLog(void){ //stand in for display run logs 
    lcd_clear();
    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    printf("  Showing Logs");
    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    printf("1. Run 1");
    lcd_set_ddram_addr(LCD_LINE3_ADDR);
    printf("2. Run 2");
    lcd_set_ddram_addr(LCD_LINE4_ADDR);
    printf("3. Run 3");
    
    while (1){
        while(PORTBbits.RB1 == 0){  continue;   }
	    unsigned char keypress = (PORTB & 0xF0) >> 4;
        while(PORTBbits.RB1 == 1){  continue;   }
        
       	Nop(); // Apply breakpoint here to prevent compiler optimizations
        
       	unsigned char temp = keys[keypress];
        if ((temp == '1')||(temp == '2')||(temp == '3')){
            //read the queue of
            lcd_clear();
            lcd_set_ddram_addr(LCD_LINE1_ADDR);
            printf("Display run");
            lcd_set_ddram_addr(LCD_LINE4_ADDR);
            printf("Press * to exit");
            while (1){
                while(PORTBbits.RB1 == 0){  continue;   }
                keypress = (PORTB & 0xF0) >> 4;
                while(PORTBbits.RB1 == 1){  continue;   }
        
                Nop();
                temp = keys[keypress];
                if (temp =='*'){
                    return;
                }
            }
        } 
        
        else{
            lcd_clear();
            lcd_set_ddram_addr(LCD_LINE1_ADDR);
            printf(" Invalid Input!");
            lcd_clear();
            lcd_set_ddram_addr(LCD_LINE1_ADDR);
            printf("  Showing Logs");
            lcd_set_ddram_addr(LCD_LINE2_ADDR);
            printf("1. Run 1");
            lcd_set_ddram_addr(LCD_LINE3_ADDR);
            printf("2. Run 2");
            lcd_set_ddram_addr(LCD_LINE4_ADDR);
            printf("3. Run 3");
        }
    }
}
    

void dateTime(void){
    //char date;
    lcd_clear();
    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    printf("Date Time");
        lcd_set_ddram_addr(LCD_LINE3_ADDR);
    printf(" Press * to end");
    while (1){
        while(PORTBbits.RB1 == 0){ continue;}
        unsigned char keypress = (PORTB & 0xF0) >>4;
        while(PORTBbits.RB1 == 1){continue;}
        
        unsigned char temp = keys[keypress];
        if (temp == '*'){
            return;
        }
    }
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              /**
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


void run(void);
void showLog(void);
void dateTime(void);


/**
 * @}
 */

#endif	/* LCD_H */
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//configBits.h
/**
 * @file
 * @author Tyler Gamvrelis
 *
 * Created on July 10, 2017, 10:54 AM
 *
 * @ingroup Config_18F4620
 */

#ifndef CONFIG_BITS_H
#define CONFIG_BITS_H

// CONFIG1H
#pragma config OSC = HS         // Oscillator Selection bits (HS oscillator)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = ON         // Data EEPROM Code Protection bit (Data EEPROM code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

#define _XTAL_FREQ 10000000    // Define osc freq for use in delay macros 

#endif /* CONFIG_BITS_H */                                                                                                                                         