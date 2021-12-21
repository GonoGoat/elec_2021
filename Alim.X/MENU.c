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
#define _XTAL_FREQ 16000000

#define PCF1_W 0b01000010   // Adresse en ?criture du PCF8574 1
#define PCF2_W 0b01000110   // Adresse en ?criture du PCF8574 2
#define PCF3_W 0b01000000   // Adresse en ?criture du PCF8574 3
#define PCF4_W 0b01001110   // Adresse en ?criture du PCF8574 4


/* PORTAbits.RA0 = Bouton Up
* PORTAbits.RA1 = Bouton OK
* PORTAbits.RA2 = Bouton BAck
* 
* PORTBbits.RB5 = LED_1
* PORTBbits.RB4 = LED_2
*/


char get_count(char compteur, char *sortie)
{       
    if (PORTAbits.RA0 == 0)
    {
        compteur++;
        __delay_ms(400);
    }
    if (PORTAbits.RA2 == 0)
    {
        __delay_ms(400);
        sortie = 1;
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

/* PORTAbits.RA0 = Bouton Up
* PORTAbits.RA1 = Bouton OK
* PORTAbits.RA2 = Bouton BAck
* 
* PORTBbits.RB5 = LED_1
* PORTBbits.RB4 = LED_2
*/
// Afficher une valeur dans un 7segments
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
    char var1,var2,sortie;
       
    afficheur(0,9);
    afficheur(1,9);
    afficheur(2,9);
    afficheur(3,9);
    while(1==1)
    {
        
    
        afficheur(0,nb_position);
        nb_position = (get_count(nb_position,&sortie)%7);   // Si 5 up est d�pass� alors retour nb_position � 0
        if(PORTAbits.RA1 == 0)
        {
            
            __delay_ms(200);
            compteur = 0;
            sortie= 0; 
            switch (nb_position)
            {
                case 0 :        // Affichage Alimentation
                    afficheur(0,0);
                    __delay_ms(2000);
                    while(sortie!=1)
                    {
                        afficheur(1,compteur);
                        compteur = get_count(compteur,&sortie)%3; 
                        if(PORTAbits.RA1 == 0)
                        {
                            while(PORTAbits.RA2 != 0)
                            {
                                switch(compteur)
                                {
                                    case 0 :
                                        //affiche valeur Puissance
                                        afficheur(2,0);
                                        break;
                                    case 1 :
                                        //affiche valeur Tension
                                        afficheur(2,1);
                                        break;
                                    case 2 :
                                        //affiche valeur Intensite
                                        afficheur(2,2);
                                        break;
                                }
                                __delay_ms(200);
                            }   
                            afficheur(0,9);     // Remets les 7 segment � 0
                            afficheur(1,9);
                            afficheur(2,9);
                            afficheur(3,9);
                            
                        }   // back 2 fois
                    }
                    break;
                    
                case 1 :        // Commande Alimentation
                    afficheur(2,1);
                    __delay_ms(2000);
                    while(sortie!=1)
                    {
                        afficheur(3,compteur);
                        compteur = get_count(compteur,&sortie)%2;

                        if(PORTAbits.RA1 == 0)
                        {
                            while (PORTAbits.RA2 != 0)
                            {
                                switch(compteur)
                                {
                                    case 0 :        // MARCHE/ARRET
                                       
                                        if(compteur_ON_OFF==0)
                                        {
                                            //affiche ON OFF
                                            //        **
                                            afficheur(1,1);
                                            afficheur(0,0);
                                        }
                                        else
                                        {
                                            //affiche ON OFF
                                            //           ***
                                            afficheur(1,1);
                                            afficheur(0,1);
                                        }
                                        if(PORTAbits.RA1 == 0)
                                        {
                                            compteur_ON_OFF =(compteur_ON_OFF +1)%2;
                                            // Allumer ou �teindre la machine (faire l'inverse)
                                        }    
                                        break;
                                        
                                    case 1 :        //REGLAGE TENSION
                                        var1 = 0,var2 = 0;
                                        while(sortie!=1)
                                        {
                                            afficheur(1,var1);
                                            var1 = (get_count(var1,&sortie)%10);    // Obtiens la dizaine de la tension voulu
                                            if(PORTAbits.RA1 == 0)
                                            {
                                                while(sortie!=1)
                                                {
                                                    var2 = (get_count(var2,&sortie)%10);
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
                            afficheur(2,0);// Remets les 7 segment � 0
                            afficheur(3,0);
                        }
                    }
                    break;
                case 2 :     // Affichage l'heure
                    afficheur(2,2);
                    while(sortie!=1)
                    {
                        compteur = get_count(compteur,&sortie);
                        // afficher l   
                    }
                    break;
                    
                case 3 :            // R�glage Date et heure 
                    afficheur(2,3);
                    __delay_ms(2000);
                    while(sortie!=1)
                    {
                        compteur = get_count(compteur,&sortie);
                    }
                    break;
                    
                case 4 :            // Lecture EEPROM
                    afficheur(2,4);
                    __delay_ms(2000);
                    while(sortie!=1)
                    {
                        compteur = get_count(compteur,&sortie);
                    }
                    break;
                    
                case 5 :            // Effacement EEPROM
                    afficheur(2,5);
                    __delay_ms(2000);
                    while(PORTAbits.RA2 == 0)
                    {
                        // affiche �tes vous sur d'effacer l'EEPROM
                        // Trouver un moyen d'afficher la phrase de comfirmation
                        if(PORTAbits.RA1 == 0)
                        {
                            //effacer l'EEPROM
                            // affiche EPPROM effac�
                            afficheur(0,9);
                        }
                    }
                    afficheur(0,0);
                    break;
                 
                case 6 :            // R�glage EEPROM
                    //enregistrement de l'EEPROM
                    afficheur(2,6);
                    __delay_ms(2000);
                    while(sortie!=1)
                    {
                        Reglage_EEPROM = get_count(Reglage_EEPROM,&sortie)%11;
                        afficheur(1, Reglage_EEPROM/10);
                        afficheur(0, Reglage_EEPROM%10);
                        if(PORTAbits.RA1 == 0)
                        {
                              // faire le reglage avec l'EEPROM
                             afficheur(0,7);
                        }
                    }
                     afficheur(0,0);
                    break;    
            }
        }
    }
}