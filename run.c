/**
 * @file
 * @author Alice Liang
 *
 * Created on July 18, 2016, 12:11 PM
 * @ingroup CharacterLCD
 */

/********************************* Includes **********************************/
#include "runTest.h"

/****Consts and Global Variables****/
const int turnTime = 750;//1428;//1795;//for arena 
const int turnTime2 = 700;
const int straightTime=1500;//3500;
const int scanTime=20;//scan time in milliseconds

const int upperlim = 25;//dist ranges for determining canisters
const int lowerlim = 0;
const int openlim = 25;//dist ranges for determining open canisters
const int openlim2 = 12;

//const int speedL=30;
//int speedR=33;

int count = 0;//timer counter
int rtn = 0;//0 for there, 1 for return
int ultSonic=0;

//int encoderCountL=0;
//int encoderCountR=0;
//int encEnable=1;

const char keys[] = "123A456B789C*0#D"; 

/****Local Function Declarations****/

/** @brief Returns 1 if there is a canister present*/
int isCanister(void);
/** @brief Returns 1 if the distance b/w the prev canister and the current canister > 20cm*/
int isDist(int *prevDist,int *dist);
/** @brief Returns 1 if canister is closed/left facing*/
int orientation(void);
/** @brief Returns 1 if there is no ball present in canister*/
int canisterStatus(void);
/** @brief Uses distance to determine if the end of the straight path has been reached*/
int isEnd(void);
/** @brief Displays message on the termination of the run and stores logs*/
void dispTermMessage(void);
/** @brief initiates timer and emergency stop interrupts*/
void initInterrupts(void);
/** @brief set led flags*/
void ledFlag(int canister);
/** @brief returns the difference between current distance and prev dist*/
int diff(int prevCan);
/** @brief scans canisters and returns the ball status*/
int scanCanister(void);
/** @brief adjusts the speed of the robot to drive straight*/
void driveStraight(void);

void run(void){ //temp stand in for running autonomous routine
    int atCanister = 0; //is there a canister detected: 0 no, 1 yes
    
    int distCheck = 0; //1 is >20cm
	int dist = 0; //var for distance
    int prevDist = 0;
    int prevCan = 0;//distance of previous canister
    
	int deployBall = 0; //should a ball be deployed: 0 no, 1 yes
	int atEnd = 0; // reused variable, 0 for there 1 for return
	int open = 0;//1 = closed, in Q: open =right, closed=left
	int flag = 0;//do not check on return, 1 is check
    //int driveCount=0;//do not adjust the driving every loop
    int dist2=0;
    int full;
    
    
	initRun();
    while (1){
        atCanister = isCanister();
        ledFlag(atCanister);
        lcd_clear();
        lcd_set_ddram_addr(LCD_LINE1_ADDR);
        printf("Dist: %d",ultSonic);
        lcd_set_ddram_addr(LCD_LINE2_ADDR);
        printf("EncR: %d",dist);
        dist2=requestArduino('A');//encoderCountR;//
        lcd_set_ddram_addr(LCD_LINE3_ADDR);
        printf("EncL: %d",dist2);
        lcd_set_ddram_addr(LCD_LINE4_ADDR);
        //printf("SpeedR: %d",speedR);
        if ((atCanister == 1)&&(rtn==0)){
            
            lcd_clear();
            lcd_set_ddram_addr(LCD_LINE1_ADDR);
            printf("Canister");
			runMotor(1); //pause motors
			distCheck = isDist(&prevCan,&dist);//check if distance exceeds 20 cm returns 1 if true
            if (distCheck == 1){
                lcd_set_ddram_addr(LCD_LINE2_ADDR);
                printf("Far Enough");
				open = orientation();//checks orientation of canister, returns 1 if 1 if open to left
				if (open == 0){
                    lcd_clear();
                    lcd_set_ddram_addr(LCD_LINE3_ADDR);
                    printf("open");
                    __delay_ms(50);
					deployBall = scanCanister(); //check conditions: is a ball inside
					if (deployBall != 0){
                        lcd_clear();
                        lcd_set_ddram_addr(LCD_LINE1_ADDR);
                        printf("Ball");
						dropBall();//deploys servo to drop ball
					}
				}
                else{
                    lcd_clear();
                    lcd_set_ddram_addr(LCD_LINE1_ADDR);
                    printf("Closed");
                    __delay_ms(50);//for testing and debugging
                }
			}
            else{
                lcd_set_ddram_addr(LCD_LINE2_ADDR);
                printf("Not far Enough");
            }
            if ((distCheck == 1)&&(open==1)){flag=1;}
			else {flag=0;}
			push(flag,open); //info for return
            dist=requestArduino('2');//encoderCountL;//
			enqueue(dist*3/4, deployBall, distCheck);//add to queue for run stats 
			runMotor(2);//restart motors
            __delay_ms(scanTime+500);//clear can
        }
        else if ((atCanister ==1)&&(rtn==1)){
            lcd_clear();
            lcd_set_ddram_addr(LCD_LINE1_ADDR);
            printf("Rtn Can");
			runMotor(1);
			flag=pop();
            lcd_set_ddram_addr(LCD_LINE2_ADDR);
            printf("Flag: %d",flag);
            __delay_ms(50);
			if (flag==1){
				deployBall = canisterStatus();
				if (deployBall==1){
                    printf("Ball");
					dropBall();
                    full=0;
				}
                else{full=1;}
			}
            
            else if (flag==-1){
                open = orientation();//checks orientation of canister, returns 1 if 1 if open to left
				if (open == 0){		
                    printf("open");
                    __delay_ms(50);
					deployBall = 1;// canisterStatus(); //check conditions: is a ball inside
					if (deployBall != 0){
                        printf("ball");
						dropBall();//deploys servo to drop ball
					}
				}
            }
            
            adjQ(deployBall,full);//adjusts the logged values for the queue
			runMotor(2);
            __delay_ms(scanTime+500);//clear can
        }
        atEnd = isEnd();
		if ((atEnd == 1)&&(rtn == 0)){
			atEnd=0;
            rtn=1;
            lcd_clear();
            lcd_set_ddram_addr(LCD_LINE1_ADDR);
            printf("Turning");
            lcd_set_ddram_addr(LCD_LINE2_ADDR);
            printf("enc: %d",dist);
			turnAround(); //initiate turn
            lcd_clear();
            lcd_set_ddram_addr(LCD_LINE1_ADDR);
            printf("Turn over");
		}
		else if ((atEnd == 1)&&(rtn == 1)){
            lcd_clear();
            lcd_set_ddram_addr(LCD_LINE1_ADDR);
            printf("END: %d",atEnd);
            lcd_set_ddram_addr(LCD_LINE2_ADDR);
            printf("enc: %d",dist);
            runMotor(4);
			dispTermMessage();//end of run generate return message
			return;
        }
        ledFlag(0);
        //if (driveCount%2==0){
        //    driveStraight();
        //    speedR=33;
        //    runMotor(1,speedL,1,speedR);
        //    driveCount=driveCount+1;
        //}
        //else if (driveCount%15==0){
        //    speedR=35;
        //    runMotor(1,speedL,1,speedR);
        //    driveCount=driveCount+1;
        //}
        //else{
        //    driveCount=driveCount+1;
        //}
        dist=requestArduino('2');//encoderCountL;//
    }    
}

//<editor-fold defaultstate="collapsed" desc=" initInterrupts ">
void initInterrupts(void){//TMR0ON=1; 
	//INTCONbits.GIE=1;

	//timer interrupts
    //TMR0IE=1;//enable timer interrupt
    //T0CON=0x07;// Prescaler= 1:256, 16-bit mode, Internal Clock
	//TMR0=0x4144;//set overflow such that there are 5 interrupts/second
    
    //encoder interrupts
    //TRISB=0x0;//Set all b pins to output
    //TRISBbits.TRISB7=1;//set B7 and B5 to inputs
    //TRISBbits.TRISB5=1;
    //INTEDG0=1;//rising edge
    //INTEDG1=1;
    //RBIE=1;//enable interrupts on change
    
    //emergency stop
	//INTCON3bits.INT2IE =1;
	//TRISBbits.RB2=1;
}
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc=" orientation ">
int orientation(void){
    //unsigned int ultSonic;
	//ultSonic = requestArduino('1');

	if ((ultSonic<openlim)&&(ultSonic>openlim2)){//check distance against tested range
		return 0;
	}
	else{
		return 1;
	}
}
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc=" initRun ">
void initRun(void){
    LATA = 0x0;
	TRISA = 0x0;//motor driving pins
    LATEbits.LATE0 = 0;//active low reset of arduino
    __delay_ms(5);
    LATEbits.LATE0 = 1;
    LATAbits.LATA4=1;//KPD=1, disable keypad
    //initPWM();
    //initInterrupts();
    initStacks();
    initDispenser();
    initArduino();
    TMR0ON=1;//start timer
    sendToArduino('8');//clear arduino values
    //runMotor(1,speedL,1,speedR);//start driving
    TRISCbits.TRISC2 = 1;//run arduino done flag
    LATAbits.LATA1 = 1;
    LATAbits.LATA2 = 1;//set start condition on run arduino
}
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc=" runMotor ">
void runMotor(int index){//1 forwards, 0 stop, 2 backwards
    if (index == 1){//start can
        LATAbits.LATA0 = 1;
        LATAbits.LATA1 = 1;
        LATAbits.LATA2 = 0;
        __delay_ms(1);
        LATAbits.LATA0 = 0;//triggers interrupt on falling edge
    }
    
    else if (index == 2){//stop can
        LATAbits.LATA1=0;
    }
    
    else if (index == 3){//turn
        LATAbits.LATA0 = 1;
        LATAbits.LATA1 = 0;
        LATAbits.LATA2 = 1;
        __delay_ms(1);
        LATAbits.LATA0 = 0;
        __delay_ms(10);//give arduino time to reset done flag
        while (PORTCbits.RC2==0){continue;}
    }
    
    else if (index == 4){//end
        LATAbits.LATA0 = 1;
        LATAbits.LATA1 = 1;
        LATAbits.LATA2 = 1;
        __delay_ms(1);
        LATAbits.LATA0 = 0;
    }
	return;
}
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc=" isCanister ">
int isCanister(void){
    //unsigned int ultSonic;
	ultSonic = requestArduino('1');

	if ((ultSonic<upperlim)&&(ultSonic>lowerlim)){
		return 1;
	}
	else{
		return 0;
	}
}
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc=" isDist ">
int isDist(int *prevDist,int *dist){
	int diff;
    int pDist = *prevDist;
    *prevDist = *dist;
	*dist = requestArduino('2');//encoderCountL;// 
	diff=*dist - pDist;
	if (diff<15){//20){
		return 0;
	}
	else{
		return 1;
	}
}
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc=" canisterStatus ">
int canisterStatus(void){
	int ballPresent=requestArduino('3');
		return ballPresent;
}
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc=" isEnd ">
int isEnd(void){
    int distance = requestArduino('2');//encoderCountL;//= 
	if ((rtn==0)&&(distance>300)){//400)){
		return 1;
	}
	else if((rtn==1)&&(distance>600)){//800)){
		return 1;
	}
	else{
		return 0;
	}
}
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc=" turnAround ">
void turnAround(void){
    //TMR0ON=0;//prevent timer from interrupting turn sequence
    sendToArduino('4');//encEnable=0;//pause encoder
	runMotor(3);
    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    printf("Straight");
	__delay_ms(straightTime);
    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    printf("Turning");
	runMotor(3);
	sendToArduino('5');//encEnable=1;//restart encoder
	return;
}
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc=" dispTermMessage ">
void dispTermMessage(void){
    TMR0ON=0;//stop timer
    unsigned int time = requestArduino('9');//count*5+7-(TMR0/9766);
    int timeM = (time)/60;
    int timeS = (time%60);
    int fullCan[10];
    int fullCount=0;
    int emptCan[10];
    int emptCount = 0;
    int dispCan[10];
    int dispCount = 0;
    int distCan[10];
    int dist;
    int ball;
    int full;
    int totCan=qLen();
	
    LATAbits.LATA4=0;//enable keypad
	for (int i=0;i<totCan;i++){
		dequeue(&dist,&ball,&full);
        distCan[i]=dist;
        if (full==1){
            fullCan[fullCount]=i+1;
            fullCount=fullCount+1;
        }
        else{
            emptCan[emptCount]=i+1;
            emptCount=emptCount+1;
        }
        if (ball==1){
            dispCan[dispCount]=i+1;
            dispCount=dispCount+1;
        }
	}
    
    writeLog((unsigned int) totCan,time, (unsigned int) fullCount, (unsigned int) emptCount, (unsigned int) dispCount);
    
    lcd_clear();
    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    printf("    Run Over");
    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    printf("Show run stats?");
    lcd_set_ddram_addr(LCD_LINE3_ADDR);
    printf("A. Yes");
    lcd_set_ddram_addr(LCD_LINE4_ADDR);
    printf("B. No");
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
	        if (temp == 'A'){ 
                lcd_clear();
                lcd_set_ddram_addr(LCD_LINE1_ADDR);
                printf("    Run Over");
                lcd_set_ddram_addr(LCD_LINE2_ADDR);
                printf("  %d canisters",totCan);
    
                if (timeS>9){
                    lcd_set_ddram_addr(LCD_LINE3_ADDR);
                    printf(" Run time %d:%d",timeM,timeS);
                }
                else {
                    lcd_set_ddram_addr(LCD_LINE3_ADDR);
                    printf(" Run time %d:0%d",timeM,timeS);
                }
                lcd_set_ddram_addr(LCD_LINE4_ADDR);
                printf("%d balls dropped",dispCount);
    
                __delay_ms(4000);
    
                lcd_clear();
                lcd_set_ddram_addr(LCD_LINE1_ADDR);
                printf("Filled Canisters");
                lcd_set_ddram_addr(LCD_LINE2_ADDR);
                if (fullCount<8){
                    for (int i=0;i<fullCount;i++){
                        printf("%d ",fullCan[i]);
                    }
                }
                else{
                    lcd_set_ddram_addr(LCD_LINE2_ADDR);
                    printf("%d %d %d %d %d %d %d %d ",fullCan[0],fullCan[1],fullCan[2],fullCan[3],fullCan[4],fullCan[5],fullCan[6],fullCan[7]);
                    lcd_set_ddram_addr(LCD_LINE3_ADDR);
                    for (int i=8;i<fullCount;i++){
                        printf("%d ",fullCan[i]);
                    }
                }
                __delay_ms(4000);
    
                lcd_clear();
                lcd_set_ddram_addr(LCD_LINE1_ADDR);
                printf(" Empty Canisters");
                lcd_set_ddram_addr(LCD_LINE2_ADDR);
                if (emptCount<8){
                    for (int i=0;i<emptCount;i++){
                        printf("%d ",emptCan[i]);
                    }
                }
                else{
                    lcd_set_ddram_addr(LCD_LINE2_ADDR);
                    printf("%d %d %d %d %d %d %d %d ",emptCan[0],emptCan[1],emptCan[2],emptCan[3],emptCan[4],emptCan[5],emptCan[6],emptCan[7]);
                    lcd_set_ddram_addr(LCD_LINE3_ADDR);
                    for (int i=8;i<emptCount;i++){
                        printf("%d ",emptCan[i]);
                    }
                }
                __delay_ms(4000);
    
                lcd_clear();
                lcd_set_ddram_addr(LCD_LINE1_ADDR);
                printf(" Can. Distances");
                lcd_set_ddram_addr(LCD_LINE2_ADDR);
                if (totCan<4){
                    for (int i=0;i<totCan;i++){
                    printf("%d ",distCan[i]);
                    }
                }
                else if((totCan>3)&&(totCan<8)){
                    lcd_set_ddram_addr(LCD_LINE2_ADDR);
                    printf("%d %d %d %d",distCan[0],distCan[1],distCan[2],distCan[3]);
                    lcd_set_ddram_addr(LCD_LINE3_ADDR);
                    for (int i=4;i<totCan;i++){
                        printf("%d ",distCan[i]);
                    }
                }
                else{
                    lcd_set_ddram_addr(LCD_LINE2_ADDR);
                    printf("%d %d %d %d",distCan[0],distCan[1],distCan[2],distCan[3]);
                    lcd_set_ddram_addr(LCD_LINE3_ADDR);
                    printf("%d %d %d %d",distCan[4],distCan[6],distCan[5],distCan[7]);
                    lcd_set_ddram_addr(LCD_LINE4_ADDR);
                    for (int i=8;i<totCan;i++){
                        printf("%d ",distCan[i]);
                    }
                }
    
                __delay_ms(4000);
    
                return;
            }
            
            else if (temp == 'B'){
                return;
            }
            else{
                lcd_clear();
                lcd_set_ddram_addr(LCD_LINE1_ADDR);
                printf("Err Invalid Input");
                lcd_clear();
                lcd_set_ddram_addr(LCD_LINE1_ADDR);
                printf("    Run Over");
                lcd_set_ddram_addr(LCD_LINE2_ADDR);
                printf("Show run stats?");
                lcd_set_ddram_addr(LCD_LINE3_ADDR);
                printf("A. Yes");
                lcd_set_ddram_addr(LCD_LINE4_ADDR);
                printf("B. No");
                __delay_ms(1000);
            }
    }
}
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc=" interruptHandler ">
void __interrupt() interruptHandler(void){
	if (INT2IF && PORTBbits.RB2){//RB2 Interrupt Emergency Stop
        	INT2IF=0; //clear flag
	        //runMotor(0,0,0,0);
        	//return to main
            lcd_clear();
            lcd_set_ddram_addr(LCD_LINE1_ADDR);
            printf(" Emergency Stop");
            lcd_set_ddram_addr(LCD_LINE3_ADDR);
            printf("  Press master ");
            lcd_set_ddram_addr(LCD_LINE4_ADDR);
            printf("  clear to exit");
            while(1){continue;}
    	}
	//else if(TMR0IF){//Timer Interrupt
    //    runMotor(0,0,0,0);
    //    lcd_clear();
    //    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    //    printf("TIMER: %d",count);
	//	TMR0IF=0;//clear interrupt
	//	TMR0=0x4144;
	//	count=count+1;
		//if (count==34){//2:50 min = 170s/5 = 850 timer interrupts
		//	count=0;
        //	if (rtn==0){
                //runMotor(2,100,2,100);
                //while (isEnd()==0){continue;}
		//	}
		//	else{
                    //runMotor(1,100,1,100);
                    //while (isEnd()==0){continue;}
		//	}
		//}
        //else{
    //        TMR0ON=1;
        //}
    //    runMotor(1,speedL,1,speedR);
	//}
    
    //else if ((RBIF && PORTBbits.RB7)&& encEnable){
    //    RBIF=0;//clear interrupt flag
    //    encoderCountL=encoderCountL+1;//increment counter
    //}
    //else if ((RBIF && PORTBbits.RB5)&& encEnable){
    //    RBIF=0;
    //    encoderCountR=encoderCountR+1;
    //}
}
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc=" ledFlags ">
void ledFlag(int canister){
    LATAbits.LATA5 = canister;
}
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc=" scanCanister ">
int scanCanister(void){
    int can=0;
    //runMotor(1,speedL,1,speedR);
    //sendToArduino('3');
    //__delay_ms(scanTime);
    //runMotor(0,0,0,0);
    can=requestArduino('6');
	return can;
}
//</editor-fold>

