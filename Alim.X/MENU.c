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
* PORTAbits.RA1 = Bouton OK
* PORTAbits.RA2 = Bouton BAck
* 
* PORTBbits.RB5 = LED_1
* PORTBbits.RB4 = LED_2
*/


char get_count(char compteur)
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
    
    char nb_position = 0;
    char compteur;   // compteur pour les rubriques
    char compteur_0N_OFF = 0;  // compteur pour la marche/arret
    char Reglage_EEPROM = 1;
/*    int nb_menu_P = 6;
    int nb_sous_r0 =3;
    int nb_sous_r1 =3;
    int nb_sous_r2 =3;
    int nb_sous_r3 =3;
    int nb_sous_r4 =3;
    int nb_sous_r5 =3;
    nb_position = get_count(nb_position);
        if ( nb_position >= nb_menu_P+nb_sous_r0+nb_sous_r1+nb_sous_r2)
            nb_position = nb_menu_P+nb_sous_r0+nb_sous_r1;
*/    
    
    while(1==1)
    {
        affiche(nb_position);
        nb_position = (get_count(nb_position)%nb_menu_P);
        if(PORTAbits.RA1 == 0)
        {
            __delay_ms(200);
            compteur = 0;
            switch (nb_position)
            {
                case 0 :        // Affichage Alimentation
                    while(compteur!=-1)
                    {
                        compteur = get_count(compteur);
                        if(compteur>=3)
                            compteur = 0;
                        if(compteur=0)
                            //affiche Puissance
                        if(compteur=1)
                            //affiche Tension
                        if(compteur=2)
                            //affiche Intensité
                            
                        if(PORTAbits.RA1 == 0)
                        {
                            while (PORTAbits.RA2 != 0)
                            {
                                switch(compteur)
                                {
                                    case 0 :
                                        //affiche valeur Puissance
                                        break;
                                    case 1 :
                                        //affiche valeur Tension
                                        break;
                                    case 2 :
                                        //affiche valeur Intensité
                                        break;
                                }
                                __delay_ms(200);
                            }   
                        }
                    }
                    break;
                    
                case 1 :        // Commande Alimentation
                    while(compteur!=-1)
                    {
                        compteur = get_count(compteur);
                        if(compteur>=2)
                            compteur = 0;
                        if(compteur=0)
                            //affiche marche - arret
                        if(compteur=1)
                            //affiche Réglage Tension
                        if(PORTAbits.RA1 == 0)
                        {
                            while (PORTAbits.RA2 != 0)
                            {
                                switch(compteur)
                                {
                                    case 0 :
                                       
                                        if(compteur_ON_OFF==0)
                                            
                                            //affiche ON OFF
                                            //        **
                                        else
                                            //affiche ON OFF
                                            //           ***
                                        
                                        if(PORTAbits.RA1 == 0)
                                        {
                                            compteur_ON_0FF =(compteur_ON_OFF +1)%2 
                                            // Allumer ou éteindre la machine
                                        }    
                                        break;
                                        
                                    case 1 :
                                        char_1 = (get_count(char_1)%10);
                                        //affiche char_1
                                        if(PORTAbits.RA1 == 0)
                                        {
                                            while (PORTAbits.RA2 == 0)
                                            {
                                                char_2 = (get_count(char_2)%10);
                                                //affiche char_2
                                                if(PORTAbits.RA1 == 0)
                                                {
                                                    //char_tension = char_1*10+char_2
                                                    //Definir tention = char_tension
                                                    __delay_ms(200);
                                                }
                                            }   // back 2 fois 
                                        } 
                                        break;
                                }
                            }
                    }
                    break;
                    
                case 2 :            // Réglage Date et heure 
                    
                    while(compteur!=-1)
                    {

                    }
                    break;
                    
                case 3 :            // Lecture EEPROM
                    while(compteur!=-1)
                    {
                        
                    }
                    break;
                    
                case 4 :            // Effacement EEPROM
             
                    while(PORTAbits.RA2 == 0)
                    {
                        // affiche Êtes vous sur d'effacer l'EEPROM
                        if(PORTAbits.RA1 == 0)
                        {
                            //effacer l'EEPROM
                            // affiche EPPROM effacé
                        }
                    }
                    break;
                 
                case 5 :            // Réglage EEPROM
                    while(PORTAbits.RA2 == 0)
                    {
                        Reglage_EEPROM = get_count(Reglage_EEPROM)%11;
                        // affiche compteur
                        if(PORTAbits.RA1 == 0)
                        {
                              // faire le reglage avec l'EEPROM
                        }
                    }
                    break;    
            }
        }   
    }
    return;
}