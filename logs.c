/**
 * @file
 * @author Alice Liang
 * @author https://www.microchip.com/forums/m/tm.aspx?m=969455&p=1&fbclid=IwAR12VkkCev5wkWkYTvZEhmg6asht9olhsGbsD9bwZTrliR6TRffwi1_ITl0
 *
 * Created on Feb 22, 2019, 10:57 AM
 * 
 * @defgroup Logs/EEProm
 * @brief reads, writes logs to EEPROM and displays the logs 
 * 
 * Preconditions:
 * @pre LCD connected to display logs
 */


#include "logs.h"


unsigned int address [4]= {0x10,0x60,0xB0,0x100};
int current;

void writeLog(unsigned int can, unsigned int time, unsigned int fullCan, unsigned int emptCan, unsigned int ball){
    current = EEPROM_Read((char)0x0);
    if (current>=3){current=0;}
    else{current = current+1;}
    EEPROM_Write((char)0x0, (unsigned int)current);
    unsigned int add = address[current];
    EEPROM_Write((unsigned char)add,can);
    EEPROM_Write((unsigned char)(add + 0x10),(unsigned char)time);
    EEPROM_Write((unsigned char)(add + (0x20)),(unsigned char)fullCan);
    EEPROM_Write((unsigned char)(add + (0x30)),(unsigned char)emptCan);
    EEPROM_Write((unsigned char)(add + (0x40)),(unsigned char)ball);
    
}

void EEPROM_Write( unsigned char address,unsigned char data ){
    if (address==(unsigned char)0x100){
        EEADR=0x0;
        EEADRH=0x1;
    }
    else{
        EEADR = address;
    }
    EEDATA = data;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1;
    INTCONbits.GIE = 0;
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    while(EECON1bits.WR); //Wait till the write completion
    EECON1bits.WREN = 0;
}

void readLog(int index, unsigned int* can, unsigned int* time, unsigned int* fullCan, unsigned int* emptCan, unsigned int* ball){
    current = (int)EEPROM_Read((unsigned char)0x0);
    if (current>0x3){current=0;}
    int a = current+index;
    if (a>3){a=current-index;}
    unsigned int add = address[a];
    *can = (int)EEPROM_Read((unsigned char)add);
    *time =(int) EEPROM_Read((unsigned char)(add + 0x10));
    *fullCan = (int)EEPROM_Read((unsigned char)(add + (0x20)));
    *emptCan = (int)EEPROM_Read((unsigned char)(add + (0x30)));
    *ball = (int)EEPROM_Read((unsigned char)(add + (0x40)));
    
}

unsigned char EEPROM_Read( unsigned char address ){
    EEADR = address;
    EECON1bits.CFGS = 0;
    EECON1bits.EEPGD = 0;
    EECON1bits.RD = 1;
    Nop(); //Nop may be required for latency at high frequencies
    Nop(); //Nop may be required for latency at high frequencies
    return ( EEDATA ); // return with read byte
}

void dispLog(char index){
    unsigned int time;
    unsigned int fullCan=0;
    unsigned int emptCan = 0;
    unsigned int ball = 0;
    unsigned int Can;
	
    readLog(index,&Can,&time,&fullCan, &emptCan, &ball);
    unsigned int timeM = (time)/60;
    unsigned int timeS = ((time)%60);
	
	lcd_clear();
	lcd_set_ddram_addr(LCD_LINE1_ADDR);
    printf("     Run %d",index);
	lcd_set_ddram_addr(LCD_LINE2_ADDR);
    printf("  %d canisters",Can);
    
    if (timeS>9){
        lcd_set_ddram_addr(LCD_LINE3_ADDR);
        printf(" Run time %d:%d",timeM,timeS);
    }
    else {
        lcd_set_ddram_addr(LCD_LINE3_ADDR);
        printf(" Run time %d:0%d",timeM,timeS);
    }
	lcd_set_ddram_addr(LCD_LINE4_ADDR);
    printf("%d balls dropped",ball);
    
	__delay_ms(4000);
    
    lcd_clear();
	lcd_set_ddram_addr(LCD_LINE1_ADDR);
    printf("Filled Canisters");
    
    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    printf("     %d",fullCan);
    
    lcd_set_ddram_addr(LCD_LINE3_ADDR);
    printf(" Empty Canisters");
    lcd_set_ddram_addr(LCD_LINE4_ADDR);
    printf("     %d",emptCan);    
    __delay_ms(4000);
    lcd_clear();
    
	return;
}