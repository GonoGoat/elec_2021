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
// Afficher un nombre dans LES 7segments
void affiche(int nombre){ 
    afficheur(3, nombre % 10);  // Unite
    afficheur(2, nombre / 10);  // Dizaine
    afficheur(1, nombre / 100);
    afficheur(0, nombre / 1000); 
}

// Afficher une valeur dans un 7segments
void afficheur(char position, char valeur)
{
    char pos_segment[16]={0b01000100,0b11110101,0b10001100,0b10100100,0b00110101,0b00100110,0b00000110,0b11110100,0b00000100,0b00100100,0b00010100,0b00000111,0b01001110,0b10000101,0b00001110, 0b00011110};
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

void main(void) 
{
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
    char compteur_ON_OFF = 0;  // compteur pour la marche/arret
    char Reglage_EEPROM = 1;
    char var1 = 0, var2 =0;

    
    
    while(1==1)
    {
        afficheur(0,nb_position);
        nb_position = (get_count(nb_position)%7);   // Si 5 up est dépassé alors retour nb_position à 0
        if(PORTAbits.RA1 == 0)
        {
            __delay_ms(200);
            compteur = 0;
            switch (nb_position)
            {
                case 0 :        // Affichage Alimentation
                    afficheur(0,1);
                    __delay_ms(2000);
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
                    afficheur(0,1);
                    __delay_ms(2000);
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
                                        {
                                            //affiche ON OFF
                                            //        **
                                        }
                                        else
                                        {
                                            //affiche ON OFF
                                            //           ***
                                        }
                                        if(PORTAbits.RA1 == 0)
                                        {
                                            compteur_ON_OFF =(compteur_ON_OFF +1)%2;
                                            // Allumer ou éteindre la machine (faire l'inverse)
                                        }    
                                        break;
                                        
                                    case 1 :
                                        var1 = (get_count(var1)%10);
                                        //affiche var1
                                        if(PORTAbits.RA1 == 0)
                                        {
                                            while (PORTAbits.RA2 == 0)
                                            {
                                                var2 = (get_count(var2)%10);
                                                //affiche var2
                                                if(PORTAbits.RA1 == 0)
                                                {
                                                    //var_tension = var1*10+var2
                                                    //Definir tention = var_tension
                                                    __delay_ms(200);
                                                }
                                            }   // back 2 fois 
                                        } 
                                        break;
                                }
                            }
                        }
                    }
                    break;
                case 2 :     // afficher l'heure
                    while(compteur!=-1)
                    {
                        compteur = get_count(compteur);
                        // afficher l   
                    }
                    break;
                    
                case 3 :            // Réglage Date et heure 
                    afficheur(0,2);
                    __delay_ms(2000);
                    while(compteur!=-1)
                    {
                        compteur = get_count(compteur);
                        
                        
                    }
                    break;
                    
                case 4 :            // Lecture EEPROM
                    afficheur(0,3);
                    __delay_ms(2000);
                    while(compteur!=-1)
                    {
                        compteur = get_count(compteur);
                    }
                    break;
                    
                case 5 :            // Effacement EEPROM
                    afficheur(0,4);
                    __delay_ms(2000);
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
                 
                case 6 :            // Réglage EEPROM
                    //enregistrement de l'EEPROM
                    afficheur(0,5);
                    __delay_ms(2000);
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