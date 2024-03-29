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

void runDisp(void){
    lcd_clear();
	lcd_set_ddram_addr(LCD_LINE1_ADDR);
    printf("  Starting Run");
}

void showLog(void){ //stand in for display run logs 
    lcd_clear();
    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    printf(" Select 1-4 to");
    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    printf("      logs");
    lcd_set_ddram_addr(LCD_LINE4_ADDR);
    printf("Press * to exit");
    
    while (1){
        while(PORTBbits.RB1 == 0){  continue;   }
	    unsigned char keypress = (PORTB & 0xF0) >> 4;
        while(PORTBbits.RB1 == 1){  continue;   }
        
       	Nop(); // Apply breakpoint here to prevent compiler optimizations
        
       	unsigned char temp = keys[keypress];
        if (temp=='1'){
            dispLog((unsigned char)0x0);
            return;
        } 
        
        else if (temp=='2'){
            dispLog((unsigned char)0x1);
            return;
        } 
        
        else if (temp=='3'){
            dispLog((unsigned char)0x2);
            return;
        } 
        
        else if (temp=='4'){
            dispLog((unsigned char)0x3);
            return;
        } 
        
        else if (temp=='*'){
            return;
        }
        
        else{
            lcd_clear();
            lcd_set_ddram_addr(LCD_LINE1_ADDR);
            printf(" Invalid Input!");
            __delay_us(1000000);
            lcd_clear();
            lcd_set_ddram_addr(LCD_LINE1_ADDR);
            printf(" Select 1-4 to");
            lcd_set_ddram_addr(LCD_LINE2_ADDR);
            printf("      logs");
            lcd_set_ddram_addr(LCD_LINE4_ADDR);
            printf("Press * to exit");
        }
        
    }
}
