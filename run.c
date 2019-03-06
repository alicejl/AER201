/**
 * @file
 * @author Alice Liang
 *
 * Created on July 18, 2016, 12:11 PM
 * @ingroup CharacterLCD
 */

/********************************* Includes **********************************/
#include "run.h"

/****Consts and Global Variables****/
const int turnTime =1500;
const int straightTime=1500;
const int upperlim = 16;//dist ranges for determining canisters
const int lowerlim = 3;
const int openlim = 3;//dist ranges for determining open canisters
const int openlim2 = 2;
int count = 0;//timer counter
int rtn = 0;//0 for there, 1 for return

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
/** @brief initiates LED flags*/
void ledInit(void);
/** @brief set led flags*/
void ledFlags(int canister,int open);


void run(void){ //temp stand in for running autonomous routine
    int atCanister = 0; //is there a canister detected: 0 no, 1 yes
    
    int distCheck = 0; //1 is >20cm
	int dist = 0; //var for distance
    int prevDist = 0;
    
	int deployBall = 0; //should a ball be deployed: 0 no, 1 yes
	int atEnd = 0; // reused variable, 0 for there 1 for return
	int open = 0;//1 = closed, in Q: open =right, closed=left
	int flag = 0;//do not check on return, 1 is check
    
    int full;
    
	initRun();
    while (1){
        atCanister = isCanister();
        if ((atCanister == 1)&&(rtn==0)){
			runMotor(0,0,0,0); //pause motors
			distCheck = isDist(&prevDist,&dist);//check if distance exceeds 20 cm returns 1 if true
            if (distCheck == 1){
				open = orientation();//checks orientation of canister, returns 1 if open to left
				if (open == 0){		
					deployBall = canisterStatus(); //check conditions: is a ball inside
					if (deployBall == 1){
						dropBall();//deploys servo to drop ball
					}
				}
			}
            if ((distCheck == 1)&&(open==1)){flag=1;}
			else {flag=0;}
            ledFlags(atCanister, open);
			push(flag,open); //info for return
			enqueue(dist, deployBall, distCheck);//add to queue for run stats 
			runMotor(1,50,1,50);//restart motors
        }
        else if ((atCanister ==1)&&(rtn==1)){
			runMotor(0,0,0,0);
			flag=pop();
            
			if (flag==1){
				deployBall = canisterStatus();
				if (deployBall==1){
					dropBall();
                    full=0;
				}
                else{full=1;}
                ledFlags(atCanister,0);
			}
            
            else if (flag==-1){
                open = orientation();//checks orientation of canister, returns 1 if open to left
				if (open == 0){		
					deployBall = canisterStatus(); //check conditions: is a ball inside
					if (deployBall == 1){
						dropBall();//deploys servo to drop ball
					}
				}
                ledFlags(atCanister,open);
            }
            
            adjQ(deployBall,full);//adjusts the logged values for the queue
			runMotor(1,50,1,50);
        }
        atEnd = isEnd();
		if ((atEnd == 1)&&(rtn == 0)){
			atEnd=0;
            rtn=1;
			turnAround(); //initiate turn
		}
		else if ((atEnd == 1)&&(rtn == 1)){
			dispTermMessage();//end of run generate return message
			return;
        }
    }
    
    
}

//<editor-fold defaultstate="collapsed" desc=" orientation ">
int orientation(void){
    unsigned int ultSonic;
	ultSonic = requestArduino('1');

	if ((ultSonic<openlim)&&(ultSonic>openlim2)){//check distance against tested range
		return 1;
	}
	else{
		return 0;
	}
}
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc=" initRun ">
void initRun(void){
    LATA = 0x0;
	TRISA = 0x0;//motor driving pins
    LATEbits.LATE0 = 0;//active low reset of arduino
    __delay_ms(1);
    LATEbits.LATE0 = 1;
    initPWM();
    initInterrupts();
    initStacks();
    initDispenser();
    initArduino();
    TMR0ON=1;//start timer
    runMotor(1,50,1,50);//start driving
}
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc=" runMotor ">
void runMotor(int dirL,float speedL,int dirR, float speedR){//1 forwards, 0 stop, 2 backwards
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
        LATAbits.LATA2 = 1;
        LATAbits.LATA3 = 0;
    }
    else if(dirR==0){
        LATAbits.LATA2 = 0;
        LATAbits.LATA3 = 0;
    }
    else{
        LATAbits.LATA2 = 0;
        LATAbits.LATA3 = 1;
    }
	set_pwm_duty_cycle(speedL,speedR);
	return;
}
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc=" isCanister ">
int isCanister(void){
    unsigned int ultSonic;
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
	*dist = (int)requestArduino('2');
	diff=*dist - pDist;
	if (diff<20){
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
    int distance = requestArduino('2');
	if ((rtn==0)&&(distance>400)){
		return 1;
	}
	else if((rtn==1)&&(distance>800)){
		return 1;
	}
	else{
		return 0;
	}
}
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc=" turnAround ">
void turnAround(void){
	sendToArduino('4');//pause encoder
	runMotor(2,30,1,30);
	__delay_ms(turnTime);
	runMotor(1,30,1,30);
	__delay_ms(straightTime);
	runMotor(2,30,1,30);
	__delay_ms(turnTime);
	sendToArduino('5');//restart encoder
	return;
}
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc=" dispTermMessage ">
void dispTermMessage(void){
    TMR0ON=0;//stop timer
    unsigned int time = count*5+7-(TMR0/9766);
    int timeM = (count*5+7-(TMR0/9766))/60;
    int timeS = ((count*5+7-(TMR0/9766))%60);
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

//<editor-fold defaultstate="collapsed" desc=" initInterrupts ">
void initInterrupts(void){
	INTCONbits.GIE=1;

	//emergency stop
	INTCON3bits.INT2IE =1;
	TRISBbits.RB2=1;

	//timer interrupts
    TMR0IE=1;//enable timer interrupt
    T0CON=0x07;// Prescaler= 1:256, 16-bit mode, Internal Clock
	TMR0=0x4144;//set overflow such that there are 5 interrupts/second
}
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc=" interruptHandler ">
void __interrupt() interruptHandler(void){
	if (INT2IE && INT2IF){//RB2 Interrupt Emergency Stop
        	INT2IF=0; //clear flag
	        runMotor(0,0,0,0);
        	//return to main
    	}
	else if(TMR0IF){//Timer Interrupt
		TMR0IF=0;//clear interrupt
		TMR0=0x4144;
		count=count+1;
		if (count==34){//2:50 min = 170s/5 = 850 timer interrupts
			count=0;
        		if (rtn==0){
                    runMotor(2,100,2,100);
                    while (isEnd()==0){continue;}//cannot do this with isEnd as is
			}
			else{
                    runMotor(1,100,1,100);
                    while (isEnd()==0){continue;}//cannot do this with isEnd as is
			}
		}
        else{
            TMR0ON=1;
        }
	}
}
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc=" ledInit ">
void ledInit(void){
    LATAbits.LATA4 = 0;
    LATAbits.LATA5 = 0;
    TRISAbits.TRISA4 = 0;
    TRISAbits.TRISA5 = 0;
}
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc=" ledFlags ">
void ledFlags(int canister, int open){
    LATAbits.LATA4 = canister;
    if (open==0){
        LATAbits.LATA5 = 1;
    }
    else{
        LATAbits.LATA5 = 0;
    }
    __delay_ms(250);
    LATAbits.LATA4 = 0;
    LATAbits.LATA5 = 0;
}
//</editor-fold>
