/**
 * @file
 * @author Alice Liang
 * @author https://exploreembedded.com/wiki/PIC_Internal_Eeprom
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


unsigned int address [4]= {0x10,0x60,0xB0,0x0};
int current;

void writeLog(unsigned int can, unsigned int time, unsigned int fullCan, unsigned int emptCan, unsigned int ball){
    current = EEPROM_ReadByte('0');
    if (current==3){current=0;}
    else{current = current+1;}
    EEPROM_WriteByte('0', (unsigned int)current);
    unsigned int add = address[current];
    EEPROM_WriteInt((unsigned char)add,can);
    EEPROM_WriteInt((unsigned char)(add + 0x10),time);
    EEPROM_WriteInt((unsigned char)(add + (0x20)),fullCan);
    EEPROM_WriteInt((unsigned char)(add + (0x30)),emptCan);
    EEPROM_WriteInt((unsigned char)(add + (0x40)),ball);
    
}

void EEPROM_WriteInt(unsigned char address,unsigned int val){
    unsigned char a = (val>>8)&0xFF;
    unsigned char b = val&0xFF;
    EEPROM_WriteByte(address,a);
    EEPROM_WriteByte((address+4),b);
}

void EEPROM_WriteByte(unsigned char eepromAddress, unsigned char eepromData){
    unsigned char gie_Status;
    
    while(WR){continue;};            // check the WR bit to see if a previous Write operation is in progress
    EEADR=eepromAddress;  // Write the address to EEADR.
    EEDATA=eepromData;    // load the 8-bit data value to be written in the EEDATA register.
    WREN=1;               // Set the WREN bit to enable eeprom operation.
    gie_Status = GIE;     // Copy the current Interrupt state
    GIE = 0;              // Disable the interrupts
    EECON2=0x55;          // Execute the special instruction sequence
    EECON2=0xaa;          // Refer the datasheet for more info
    WR=1;                 // Set the WR bit to trigger the eeprom write operation.
    GIE = gie_Status;     // Restore the interrupts
    WREN=0;               // Disable the EepromWrite
}

void readLog(int index, unsigned int* can, unsigned int* time, unsigned int* fullCan, unsigned int* emptCan, unsigned int* ball){
    current = EEPROM_ReadByte('0');
    int a = current+index;
    if (a>3){a=current-index;}
    unsigned int add = address[a];
    *can = EEPROM_ReadInt((unsigned char)add);
    *time = EEPROM_ReadInt((unsigned char)(add + 0x2));
    *fullCan = EEPROM_ReadInt((unsigned char)(add + (0x4)));
    *emptCan = EEPROM_ReadInt((unsigned char)(add + (0x6)));
    *ball = EEPROM_ReadInt((unsigned char)(add + (0x8)));
    
}

unsigned int EEPROM_ReadInt(unsigned char address){
    unsigned int a = EEPROM_ReadByte(address);
    unsigned int b = EEPROM_ReadByte(address+4);
    unsigned int data = a;
    data=(data<<8)|b;
    return data;
}

unsigned char EEPROM_ReadByte(unsigned char eepromAddress){
    while(RD || WR){continue;}           // check the WR&RD bit to see if a RD/WR is in progress
    EEADR=eepromAddress;       // Write the address to EEADR.
    RD = 1;                    // Set the RD bit to trigger the eeprom read operation.
    return(EEDATA);            // Return the data read form eeprom.
}

void dispLog(char index){
    unsigned int time;
    unsigned int fullCan=0;
    unsigned int emptCan = 0;
    unsigned int ball = 0;
    unsigned int Can;
	
    readLog(index,&Can,&time,&fullCan, &emptCan, &ball);
    int timeM = (time)/60;
    int timeS = ((time)%60);
	
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