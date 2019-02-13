/**
 * @file
 * @author Alice Liang
 *
 * Created on July 18, 2016, 12:11 PM
 * @ingroup CharacterLCD
 */

/********************************* Includes **********************************/
#include "run.h"



void run(void){ //temp stand in for running autonomous routine
	lcd_clear();
	lcd_set_ddram_addr(LCD_LINE1_ADDR);
    	printf("  Starting Run");
    	lcd_set_ddram_addr(LCD_LINE3_ADDR);
	printf(" Press * to end");

	runInit();
    	runMotor(1,1);
    
  	while (1){
        	while(PORTBbits.RB1 == 0){ continue;}
        	unsigned char keypress = (PORTB & 0xF0) >>4;
        	while(PORTBbits.RB1 == 1){continue;}
        
        	unsigned char temp = keys[keypress];
        	if (temp == '*'){
            	runMotor(0,0);
            	return;
		}
	}
}

void runInit(void){
	TRISAbits.RA0 = 0;
        TRISAbits.RA1 = 0;
        TRISAbits.RA2 = 0;
        TRISAbits.RA3 = 0;
        LATAbits.LATA0 = 0;
        LATAbits.LATA1 = 0;
        LATAbits.LATA2 = 0;
        LATAbits.LATA3 = 0;
}

void runMotor(int instrL,int instrR){
	if (instrL==0){
		LATAbits.LATA0 = 0;
	}
	else {
		LATAbits.LATA0 = 1;
	}
	if (instrR==0){
		LATAbits.LATA1 = 0;
	}
	else {
		LATAbits.LATA1 = 1;
	return;
}