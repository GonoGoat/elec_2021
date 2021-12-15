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


int get_count(int compteur)
{       
    if (PORTAbits.RA0 == 0)
    {
        compteur++;
        __delay_ms(200);
    }
    if (PORTAbits.RA2 == 0)
    {
        __delay_ms(200);
        return -1;
    }    
    return compteur;
}

void affiche(int nombre);
void afficheur(int position, int valeur);

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
    
    int nb_position = 0;
    int nb_menu_P = 5;
    int nb_sous_r0 =3;

    /* PORTAbits.RA0 = Bouton Up
     * PORTAbits.RA1 = Bouton OK
     * PORTAbits.RA2 = Bouton Back
     * 
     * PORTBbits.RB5 = LED_1
     * PORTBbits.RB4 = LED_2
     */
    
    while(1==1)
    {
        //affi_position_1
        //affi_position_2
        affiche(nb_position);
        nb_position = (get_count(nb_position)%nb_menu_P);
        if(PORTAbits.RA1 == 0)
        {
            __delay_ms(200);
            switch (nb_position)
            {
                
                case 0 :
                    nb_position = nb_menu_P;
                    while(nb_position!=-1)
                    {
                        //affi_position_1
                        //affi_position_2
                        affiche(nb_position);
                        nb_position = get_count(nb_position);
                        if ( nb_position >= nb_menu_P+nb_sous_r0 )
                            nb_position = nb_menu_P;
                        if(PORTAbits.RA1 == 0)
                        {
                            __delay_ms(200);
                            affiche(nb_position); 
                        }
                    }
                    
                    break;
                case 1 : 
                    nb_position = nb_menu_P+nb_sous_r0;
                    while(nb_position!=-1)
                    {
                        //affi_position_1
                        //affi_position_2
                        affiche(nb_position);
                        
                    }
                    break;
                case 2 : 
                    nb_position = nb_menu_P+nb_sous_r1;
                    while(nb_position!=-1)
                    {
                        //affi_position_1
                        //affi_position_2
                        affiche(nb_position);
                        
                    }
                    break;
                case 3 : 
                    nb_position = nb_menu_P+nb_sous_r2;
                    while(nb_position!=-1)
                    {
                        affiche(nb_position);
                        
                    }
                    break;
                case 4 : 
                    nb_position = nb_menu_P+nb_sous_r3;
                    while(nb_position!=-1)
                    {
                        //affi_position_1
                        //affi_position_2
                        affiche(nb_position);
                        
                    }
                    break;
            }
        }
        
    }
    return;
}


// Afficher un nombre dans LES 7segments
void affiche(int nombre){ 
    afficheur(3, nombre % 10);  // Unite
    afficheur(2, nombre / 10);  // Dizaine
    afficheur(1, nombre / 100);
    afficheur(0, nombre / 1000); 
}

// Afficher une valeur dans un 7segments
void afficheur(int position, int valeur){
    int pos_segment[16]={0b01000100,0b11110101,0b10001100,0b10100100,0b00110101,0b00100110,0b00000110,0b11110100,0b00000100,0b00100100,0b00010100,0b00000111,0b01001110,0b10000101,0b00001110, 0b00011110};
    SSPCON2bits.SEN = 1;                // Lancement du START
    while(PIR1bits.SSPIF == 0);         // Attente de la fin du START // On attend que PIR1bits.SSPIF passe ? 1
    PIR1bits.SSPIF = 0;                 // Remise ? 0 du flag
        
    switch(position){
        case 0:
            SSPBUF = PCF1_W;
            break;
        case 1:
            SSPBUF = PCF2_W;
            break;
        case 2:
            SSPBUF = PCF3_W;
            break;
        case 3:
            SSPBUF = PCF4_W;
            break;
    }
                                        // Envoyer l'adresse
    while(PIR1bits.SSPIF == 0);         // Attente de la fin de l'envoi de l'adresse
    PIR1bits.SSPIF = 0;                 // Remise ? 0 du flag

    SSPBUF = pos_segment[valeur % 10];  // Envoi de la donn?e
    while(PIR1bits.SSPIF == 0);         // Attente de la fin de l'envoi de la donn?e
    PIR1bits.SSPIF = 0; 
    
    SSPCON2bits.PEN = 1;                // Lancement du STOP
    while(PIR1bits.SSPIF == 0);         // Attente de la fin du STOP
    PIR1bits.SSPIF = 0;                 // Remise ? 0 du flag   
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
 * 
 * 
 * if (PORTAbits.RA2 == 0)
        {
            compteur = 0;
            PORTBbits.RB4 = 0;
            PORTBbits.RB5 = 0;
            // retourner au menu principal ou rien 
        }
        */