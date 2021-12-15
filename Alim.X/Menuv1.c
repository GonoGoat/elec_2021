// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#define _XTAL_FREQ 10000000
#define PCF1_W 0b01000000   // nième 7 digits
#define PCF2_W 0b01000110   // nième 7 digits
#define PCF3_W 0b01000000   // nième 7 digits
#define PCF4_W 0b01001110   // nième 7 digits


/* PORTAbits.RA0 = Bouton Up
* PORTAbits.RA1 = Bouton Down
* PORTAbits.RA2 = Bouton OK
* 
* PORTBbits.RB5 = LED_1
* PORTBbits.RB4 = LED_2
*/


void main(void) {
    char pos_segment[16]={0b01000100,0b11110101,0b10001100,0b10100100,0b00110101,0b00100110,0b00000110,0b11110100,0b00000100,0b00100100,0b00010100,0b00000111,0b01001110,0b10000101,0b00001110, 0b00011110};
    //Initialisations
    TRISA = 0b11111111;  //ou TRISA = 255 ou TRISA = 0xFF
    TRISB = 0b00000000; 
    TRISC = 255;
    ADCON1 = 0b00000110; //Config en I/O digitales pour toutes les pattes disponibles
    PORTB = 0b00000000;
    
    SSPSTAT = 0b11000000;
    SSPCON = 0b00101000;
    SSPCON2 = 0b00000000;
    SSPADD = 24;
    
    int compteur = 0;
    int nb_rubrique = 3;

    /* PORTAbits.RA0 = Bouton Up
     * PORTAbits.RA1 = Bouton OK
     * PORTAbits.RA2 = Bouton Back
     * 
     * PORTBbits.RB5 = LED_1
     * PORTBbits.RB4 = LED_2
     */
    
    while(1==1)
    {
        // Appel fonction d'affichage
        if(PORTAbits.RA0 == 0)
            compteur++;
        if(PORTAbits.RA1 == 0)
        {
            switch (compteur%nb_rubrique)
            {
                
                case 0 : 
                    PORTBbits.RB4 = 1;
                    // Appel fonction d'affichage
                    break;
                case 1 : 
                    PORTBbits.RB5 = 1;
                    // Appel fonction d'affichage
                    break;
                case 2 : 
                    PORTBbits.RB4 = 1;
                    PORTBbits.RB5 = 1;
                    // Appel fonction d'affichage
                    break;         
            }
        }
        if (PORTAbits.RA2 == 0)
        {
            compteur = 0;
            PORTBbits.RB4 = 0;
            PORTBbits.RB5 = 0;
            // retourner au menu principal ou rien 
        }
    }
    return;
}

/*
        PORTBbits.RB5 = 0;
        PORTBbits.RB4 = 0;
        
        if (PORTAbits.RA0 == 0)
        {
            PORTBbits.RB5 = 1;
        } 
        if (PORTAbits.RA1 == 0)
        {
            PORTBbits.RB4 = 1;
        }
        if (PORTAbits.RA2 == 0)
        {            
            PORTBbits.RB4 = 1;
            PORTBbits.RB5 = 1;
        }
        */