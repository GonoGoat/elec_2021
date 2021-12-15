/*
 * File:   main.c
 * Author: ELEC MA0/BA3
 *
 * Created on November 24, 2021, 2:23 PM
 */

// CONFIG ------------------------------------------------------------------------------------------------------------------------------------------------

#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF. --------------------------------------------------------------------------------------------------


#include <xc.h>
#define _XTAL_FREQ 16000000

#define PCF1_W 0b01000010   // Adresse en �criture du PCF8574 1
#define PCF2_W 0b01000110   // Adresse en �criture du PCF8574 2
#define PCF3_W 0b01000000   // Adresse en �criture du PCF8574 3
#define PCF4_W 0b01001110   // Adresse en �criture du PCF8574 4

void main(void) {

    while(1){        
        // TODO
    }
    return;
}