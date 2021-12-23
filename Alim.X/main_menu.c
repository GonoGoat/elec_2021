/*
 * File:   main_menu.c
 * Author: Thoma
 *
 * Created on 23 décembre 2021, 15:08
 */

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#include <xc.h>
#define _XTAL_FREQ 20000000

#define PCF1_W 0b01000010   // Adresse en ?criture du PCF8574 1
#define PCF2_W 0b01000110   // Adresse en ?criture du PCF8574 2
#define PCF3_W 0b01000000   // Adresse en ?criture du PCF8574 3
#define PCF4_W 0b01001110   // Adresse en ?criture du PCF8574 4

char tension = 0, intensite = 0, puissance = 0;

/* PORTAbits.RA0 = Bouton Up
* PORTAbits.RA1 = Bouton OK
* PORTAbits.RA2 = Bouton Back
* 
* PORTBbits.RB5 = LED_1
* PORTBbits.RB4 = LED_2
*/

char get_count(char compteur)
{      
    if (PORTAbits.RA0 == 0)
    {
        compteur++;
        __delay_ms(400);
    }
    return compteur;
}

// Afficher une valeur dans un 7segments
void afficheur(char position, char valeur)
{
    char pos_segment[16]={0b01000100,0b11110101,0b10001100,0b10100100,0b00110101,0b00100110,0b00000110,0b11110100,0b00000100,0b00100100,0b00010100,0b00000111,0b01001110,0b10000101,0b00001110, 0b00011110};
    SSPCON2bits.SEN = 1;                // Lancement du START
    while(PIR1bits.SSPIF == 0);         // Attente de la fin du START // On attend que PIR1bits.SSPIF passe ? 1
    PIR1bits.SSPIF = 0;                 // Remise ? 0 du flag
        
    switch(position)
    {
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
    tension = 43;
    intensite = 2;
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
    char compteur,compteur2,compteur3;   // compteur pour les rubriques
    char compteur_ON_OFF = 0;  // compteur pour la marche/arret
    char Reglage_EEPROM = 1;
    char fin=0;   
    
    afficheur(0,0);
    afficheur(1,0);
    afficheur(2,0);
    afficheur(3,0);
    
    while(1==1)
    {   
        nb_position = get_count(nb_position)%7;
        afficheur(0,nb_position+1);
        if(PORTAbits.RA1 == 0)
        {
            __delay_ms(400);
            compteur = 0;
            fin = 0;
            switch(nb_position)
            {
                case 0 :    // Affichage Alimentation
                    while(PORTAbits.RA2 != 0)
                    {
                        afficheur(1,compteur+1);
                        compteur = get_count(compteur)%3; 
                        if(PORTAbits.RA1 == 0)
                        {
                            while(PORTAbits.RA2 != 0)
                            {
                                switch(compteur)
                                {
                                    case 0 :
                                        puissance = intensite*tension;
                                        afficheur(0,puissance/1000);
                                        afficheur(1,(puissance/100)%10);
                                        afficheur(2,(puissance/10)%10);
                                        afficheur(3,puissance%10);
                                        //affiche valeur Puissance
                                        break;
                                    case 1 :
                                        afficheur(0,tension/1000);
                                        afficheur(1,(tension/100)%10);
                                        afficheur(2,(tension/10)%10);
                                        afficheur(3,tension%10);
                                        //affiche valeur Tension
                                        break;
                                    case 2 :
                                        afficheur(0,(intensite/1000)%10);
                                        afficheur(1,(intensite/100)%10);
                                        afficheur(2,(intensite/10)%10);
                                        afficheur(3,intensite%10);
                                        //affiche valeur Intensite
                                        break;
                                }
                            }   
                            afficheur(2,0);
                            afficheur(3,0);
                            __delay_ms(400);
                        }
                    }
                    break;
                    
                case 1 :    // Commande Alimentation
                    while(PORTAbits.RA2 != 0 && fin == 0)
                    {         
                        compteur = get_count(compteur)%2;
                        afficheur(1,compteur+1);
                        if(PORTAbits.RA1 == 0)
                        {
                            afficheur(0,0);
                            afficheur(1,0);
                            while (PORTAbits.RA2 != 0 && fin == 0)
                            {
                                switch(compteur)
                                {
                                    case 0 :        // Marche/Arret
                                        compteur_ON_OFF =get_count(compteur_ON_OFF)%2;
                                        afficheur(3,compteur_ON_OFF);
                                        if(PORTAbits.RA1 == 0)
                                        {
                                            // faire le changement : Allumer ou Éteindre
                                        }    
                                        break;
                                        
                                    case 1 :        //Commande Tension
                                        compteur2 = tension;
                                        //compteur3 = 0;
                                        __delay_ms(400); 
                                        while(PORTAbits.RA2 != 0 && fin == 0)
                                        {
                                            
                                            afficheur(2,compteur2);
                                            compteur2 = get_count(compteur2)%10;    // Obtiens la dizaine de la tension voulue
                                            if(PORTAbits.RA1 == 0)
                                            {   
                                                __delay_ms(800); 
                                                while(PORTAbits.RA2 != 0 && fin == 0)
                                                {
                                                    afficheur(3,compteur2);
                                                    compteur2 = get_count(compteur2)%10; // Obtiens l'unité de la tension voulue
                                                    if(PORTAbits.RA1 == 0)
                                                    {
                                                        //Definir tention = compteur2*10+compteur2
                                                        //tension = compteur2*10+compteur2;
                                                         fin = 1;
                                                        __delay_ms(400); 
                                                    }
                                                }                          
                                            }
                                            tension=compteur2;
                                         } 
                                        break;
                                }
                                
                            }
                            afficheur(0,nb_position+1);
                            afficheur(1,compteur+1);
                            afficheur(2,0);
                            afficheur(3,0);
                        }
                    }
                    break;
                    
                case 2 :     // Affichage Heure
                    while(PORTAbits.RA2 != 0 && fin == 0)
                    {
                        // afficher jour et mois 
                        if(PORTAbits.RA1 == 0)
                        {
                            while(PORTAbits.RA2 != 0 && fin == 0)
                            {
                                __delay_ms(400);
                                // afficher heure et minutes
                                if(PORTAbits.RA1 == 0)
                                {
                                    while(PORTAbits.RA2 != 0 && fin == 0)
                                    {
                                        __delay_ms(400);
                                        // afficher secondes
                                        if(PORTAbits.RA1 == 0)
                                        {
                                            fin = 1;
                                            __delay_ms(400);
                                        }
                                    }
                                }  
                            }
                        }
                    }
                    break;
                    
                case 3 :    // Commande Date
                    while(PORTAbits.RA2 != 0)
                    {
                        compteur = get_count(compteur)%5;
                        afficheur(1,compteur+1);
                        if(PORTAbits.RA1 == 0)
                        {
                            switch(compteur)
                            {
                                case 0:
                                    // saisie et changement du mois 
                                    break;
                                case 1:
                                    // saisie et changement du jour
                                    break;
                                case 2:
                                    // saisie et changement de l'heure 
                                    break;
                                case 3:
                                    // saisie et changement des minutes
                                    break;
                                case 4:
                                    // saisie et changement du mois secondes
                                    break;
                            }
                        }
                    }
                    break;
                    
                case 4 :    // Lecture EEPROM
                    while(PORTAbits.RA2 != 0)
                    {
                        compteur = get_count(compteur)%5;
                        afficheur(1,compteur+1);
                        if(PORTAbits.RA1 == 0)
                        {
                            while(PORTAbits.RA2 != 0)
                            {   
                                switch(compteur)
                                {
                                    case 0:
                                        // Selection de la case mémoire à atteindre
                                        break;
                                    case 1:
                                        // Lecture sur l'EEPROM de l'heure et des minutes
                                        break;
                                    case 2:
                                        // Lecture sur l'EEPROM des secondes
                                        break;
                                    case 3:
                                        // Lecture sur l'EEPROM de la tension
                                        break;
                                    case 4:
                                        // Lecture sur l'EEPROM de l'intensité
                                        break;
                                }
                            }
                            //afficheur(0,nb_position);
                            //afficheur(1,compteur);
                            //afficheur(2,0);
                            //afficheur(3,0);
                        }
                        
                    }
                    break;
                    
                case 5 :    // Effacement EEPROM
                    __delay_ms(400);
                    while(PORTAbits.RA2 != 0 && fin == 0)
                    {
                        afficheur(1,1);
                        if(PORTAbits.RA1 == 0)
                        {
                            //effacer l'EEPROM
                            fin = 1;
                            __delay_ms(400);
                        }
                    }
                    break;
                 
                case 6 :    // Commande EEPROM
                    afficheur(0,0);
                    __delay_ms(400);
                    while(PORTAbits.RA2 != 0 && fin == 0)
                    {
                        Reglage_EEPROM = get_count(Reglage_EEPROM)%11;
                        afficheur(2, Reglage_EEPROM/10);
                        afficheur(3, Reglage_EEPROM%10);
                        if(PORTAbits.RA1 == 0)
                        {
                              // faire le reglage avec l'EEPROM
                            fin = 1;
                            __delay_ms(400);
                        }
                    }
                    afficheur(2,0);
                    afficheur(3,0);                    
                    break;
            }
        }
       afficheur(1,0); 
    }         
}