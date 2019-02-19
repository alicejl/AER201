/**
 * @file
 * @author Michael Ding
 * @author Tyler Gamvrelis
 *
 * Created on July 14, 2016, 10:25 AM
 * 
 * @defgroup PWM
 * @brief Demonstrates PWM on RC2 by modulating the duty cycle with a sine wave
 */

#include <xc.h>
#include <math.h>
#include "pwm.h"

//<editor-fold defaultstate="collapsed" desc=" CONFIG BITS ">

// CONFIG1H
//#pragma config OSC = INTIO7    // Oscillator Selection bits (Internal oscillator block, port function on RA6 and RA7)
//#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
//#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = OFF         // Watchdog Timer Enable bit (WDT disabled)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

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
//</editor-fold>

#define _XTAL_FREQ 32000000

/**
 * @brief Sets the duty cycle of PWM1
 * @param duty The desired duty cycle as a percentage of the period (min: 0, 
 *        max: 100)
 */
void set_pwm_duty_cycle(float duty1,float duty2){
    if((duty1 >= 0) && (duty1 <= 100.0) && (duty2 >= 0) && (duty2 <= 100.0)){
        // Our pulse width cannot exceed the period of the wave. First we
        // compute this upper limit for the duty cycle registers (max_duty_val),
        // then we compute a percentage of this (duty_val) as per the argument
        // passed in. See datasheet pg 150-151 (equation 16-2, and figure 16-2)
        unsigned short max_duty_val = PR2 + 1;
        unsigned short duty_val1 = (unsigned short)(
            (duty1 / 100.0) * (float)max_duty_val
        );
	    unsigned short duty_val2 = (unsigned short)(
            (duty2 / 100.0) * (float)max_duty_val
        );
        
        // Save the duty cycle into the registers
        CCP1X = duty_val1 & 2; // Set the 2 least significant bit in CCP1CON register
        CCP1Y = duty_val1 & 1;
        CCPR1L = duty_val1 >> 2; // Set rest of the duty cycle bits in CCPR1L

        CCP2X = duty_val2 & 2; // Set the 2 least significant bit in CCP1CON register
        CCP2Y = duty_val2 & 1;
        CCPR2L = duty_val2 >> 2; // Set rest of the duty cycle bits in CCPR1L
    }
}

void initPWM(void){
    // Set internal oscillator to run at 8 MHZ
    OSCCON = OSCCON | 0b01110000; 
    
    // Enable PLL for the internal oscillator, the processor now runs at 32 MHz
    //OSCTUNEbits.PLLEN = 1; 
    
    // Disable output from PWM pin while we are setting up PWM
    TRISCbits.TRISC2 = 1;
    TRISCbits.TRISC1 = 1;
     
    // Configure PWM frequency, 3.1 kHz. See datasheet pg. 149, equation 16-1
    const unsigned long FREQUENCY = 3100;
    const unsigned char TIMER2_PRESCALER = 1;//16;
    PR2 = (_XTAL_FREQ / (FREQUENCY * 4 * TIMER2_PRESCALER)) - 1;
    
    // Configure CCP1CON, single output mode, all active high
    P1M1 = 0;
    P1M0 = 0;
    CCP1M3 = 1;
    CCP1M2 = 1;
    CCP1M1 = 0;
    CCP1M0 = 0;

    // Configure CCP1CON, single output mode, all active high
    CCP2M3 = 1;
    CCP2M2 = 1;
    CCP2M1 = 0;
    CCP2M0 = 0;

    // Set timer 2 prescaler to 16
    T2CKPS0 = 1;
    T2CKPS1 = 1;

    // Enable timer 2
    TMR2ON = 1;

    // Enable PWM output pin since setup is done
    TRISCbits.TRISC2 = 0;
    TRISCbits.TRISC1 = 0;
}