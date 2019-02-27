/**
 * @file
 * @author Alice Liang
 *
 * @defgroup Dispensor
 * @brief control servo and ball dispention
 */

#include <xc.h>
#include "ballDispenser.h"

int moveDelay=500;

void initDispensor(void){
    LATCbits.LATC5=0;
    TRISCbits.TRISC5=0;
    rotate90();
}

void rotate0(void){
    for (int i=0;i<40;i++){
        LATCbits.LATC5=1;
        __delay_us(1400);
        LATCbits.LATC5=0;
        __delay_us(18600);
    }
}

void rotate90(void){
    for (int i=0;i<40;i++){
        LATCbits.LATC5=1;
        __delay_us(2400);
        LATCbits.LATC5=0;
        __delay_us(17600);
    }
}

void dropBall(void){
    rotate0();
    __delay_ms(moveDelay);
    rotate90();
}