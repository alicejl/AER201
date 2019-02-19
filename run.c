/**
 * @file
 * @author Alice Liang
 *
 * Created on July 18, 2016, 12:11 PM
 * @ingroup CharacterLCD
 */

/********************************* Includes **********************************/
#include "run.h"
const char keys[] = "123A456B789C*0#D"; 

void run(void){ //temp stand in for running autonomous routine
	initRun();
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

void initRun(void){
    TRISAbits.RA0 = 0;
    TRISAbits.RA1 = 0;
    TRISAbits.RA2 = 0;
    TRISAbits.RA3 = 0;
    LATAbits.LATA0 = 0;
    LATAbits.LATA1 = 0;
    LATAbits.LATA2 = 0;
    LATAbits.LATA3 = 0;
    initPWM();
}

void runMotor(int dirL,float speedL,int dirR, float speedR){
	void runMotor(int dirL,float speedL,int dirR, float speedR){
    if (dirL==1){
        LATAbits.LATA0 = 0;
        LATAbits.LATA1 = 1;
    }
    else if(dirL==0){
        LATAbits.LATA0 = 0;
        LATAbits.LATA1 = 0;
    }
    else{
        LATAbits.LATA0 = 1;
        LATAbits.LATA1 = 0;
    }
    if (dirR==1){
        LATAbits.LATA2 = 0;
        LATAbits.LATA3 = 1;
    }
    else if(dirR==0){
        LATAbits.LATA2 = 0;
        LATAbits.LATA3 = 0;
    }
    else{
        LATAbits.LATA2 = 1;
        LATAbits.LATA3 = 0;
    }
	set_pwm_duty_cycle(speedL,speedR);
	return;
}

void turnAround(void){
	//pause odometer;
	runMotor(2,50,1,50);
	__delay_ms(turnTime);
	runMotor(1,30,1,30);
	__delay_ms(straightTime);
	runMotor(2,30,1,30);
	__delay_ms(turnTime);
    runMotor(0,30,0,30);
	//start odometer;
	return;
}
