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
#define _XTAL_FREQ 16000000

#define PCF1_W 0b01000010   // Adresse en ?criture du PCF8574 1
#define PCF2_W 0b01000110   // Adresse en ?criture du PCF8574 2
#define PCF3_W 0b01000000   // Adresse en ?criture du PCF8574 3
#define PCF4_W 0b01001110   // Adresse en ?criture du PCF8574 4
#define RTC_W 0b11010000


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

void write_rtc (char registre, char donnee)
{
        SSPCON2bits.SEN = 1;            //Lancement du START
        while(PIR1bits.SSPIF == 0);     //Attente de la fin du START
        PIR1bits.SSPIF = 0;             //Remise ? 0 du Flag
        
        SSPBUF = RTC_W;                //Envoi de l'adresse
        while(PIR1bits.SSPIF == 0);     //Attente de la fin de l'envoi de l'adresse
        PIR1bits.SSPIF = 0;             //Remise ? 0 du Flag
        
        SSPBUF = registre;
        while(PIR1bits.SSPIF == 0);     //Attente de la fin de l'envoi du registre
        PIR1bits.SSPIF = 0;             //Remise ? 0 du Flag
        
        SSPBUF = donnee;
        while(PIR1bits.SSPIF == 0);     //Attente de la fin de l'envoi de la donn?e
        PIR1bits.SSPIF = 0;             //Remise ? 0 du Flag
        
        SSPCON2bits.PEN = 1;            //Lancement du STOP
        while(PIR1bits.SSPIF == 0);     //Attente de la fin du STOP
        PIR1bits.SSPIF = 0;             //Remise ? 0 du Flag


}

char read_rtc(char registre)
{
    char donnee;
    
        SSPCON2bits.SEN = 1;            //Lancement du START
        while(PIR1bits.SSPIF == 0);     //Attente de la fin du START
        PIR1bits.SSPIF = 0;             //Remise ? 0 du Flag
        
        SSPBUF = RTC_W;                //Envoi de l'adresse en ecriture
        while(PIR1bits.SSPIF == 0);     //Attente de la fin de l'envoi de l'adresse
        PIR1bits.SSPIF = 0;             //Remise ? 0 du Flag

        SSPBUF = registre;
        while(PIR1bits.SSPIF == 0);     //Attente de la fin de l'envoi du registre
        PIR1bits.SSPIF = 0;             //Remise ? 0 du Flag
        
        SSPCON2bits.RSEN = 1;            //Lancement du REPEATED START
        while(PIR1bits.SSPIF == 0);     //Attente de la fin du START
        PIR1bits.SSPIF = 0;             //Remise ? 0 du Flag
        
        SSPBUF = RTC_W + 1;                //Envoi de l'adresse en lecture
        while(PIR1bits.SSPIF == 0);     //Attente de la fin de l'envoi de l'adresse
        PIR1bits.SSPIF = 0;             //Remise ? 0 du Flag

        
        SSPCON2bits.RCEN = 1;            //Autorisation de r?ception
        
        while(PIR1bits.SSPIF == 0);     //Attente de la fin du START
        PIR1bits.SSPIF = 0;             //Remise ? 0 du Flag
        donnee = SSPBUF;
        
        SSPCON2bits.PEN = 1;            //Lancement du STOP
        while(PIR1bits.SSPIF == 0);     //Attente de la fin du STOP
        PIR1bits.SSPIF = 0;             //Remise ? 0 du Flag

        return donnee;
}

char get_number_limit(char limite)
{
    char compteur2=0,compteur3=0;
    __delay_ms(400);
    while(PORTAbits.RA2 != 0)
    {
        afficheur(2,compteur2);
        compteur2 = get_count(compteur2)%(limite/10+1);    // Obtiens la dizaine de la tension voulue
        if(PORTAbits.RA1 == 0)
        {   
            __delay_ms(800);
            while(PORTAbits.RA2 != 0)
            {
                afficheur(3,compteur3);
                compteur3 = get_count(compteur3)%(limite-compteur2*10+1); // Obtiens l'unité de la tension voulue
                if(PORTAbits.RA1 == 0)
                {
                    __delay_ms(400);
                    return compteur2*10+compteur3;  
                }
            }                          
        }
    } 
    return 0 ;
}
void reglages_rtc (char mois, char date, char heure, char minute, char seconde)
{
    write_rtc(0,seconde);
    write_rtc(1,minute);
    write_rtc(2,heure);
    write_rtc(4,date);
    write_rtc(5,mois);
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
    char compteur,compteur2,compteur3;   // compteur pour les rubriques
    char compteur_ON_OFF = 0;  // compteur pour la marche/arret
    char Reglage_EEPROM = 1;
    char fin=0,tension,mois,jours,heures,minutes,secondes,case_eeprom;
    
    reglages_rtc(0b00010010,0b00100011,0b00010010,0b000110100,0);  // Init RTC
    
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
                                        //affiche valeur Puissance
                                        break;
                                    case 1 :
                                        //affiche valeur Tension
                                        break;
                                    case 2 :
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
                                        __delay_ms(600); 
                                        tension=get_number_limit(99);
                                        // changer tension
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
                            mois = read_rtc(5);
                            afficheur(3, mois & 0b00001111);
                            afficheur(2, (mois & 0b11110000)>>4);
                            
                            jours = read_rtc(4);
                            afficheur(1, jours & 0b00001111);
                            afficheur(0, (jours & 0b11110000)>>4);
         
                        if(PORTAbits.RA1 == 0)
                        {
                            while(PORTAbits.RA2 != 0 && fin == 0)
                            {
                                __delay_ms(400);
                                // afficher heure et minutes
                                    minutes = read_rtc(1);
                                    afficheur(3, minutes & 0b00001111);
                                    afficheur(2, (minutes & 0b11110000)>>4);
                                    heures = read_rtc(2);
                                    afficheur(1, heures & 0b00001111);
                                    afficheur(0, (heures & 0b11110000)>>4);
                                if(PORTAbits.RA1 == 0)
                                {
                                    while(PORTAbits.RA2 != 0 && fin == 0)
                                    {
                                        __delay_ms(400);
                                        // afficher secondes
                                        minutes = 0b00000000;
                                        afficheur(0, minutes & 0b00001111);
                                        afficheur(1, (minutes & 0b11110000)>>4);
                                        secondes = read_rtc(0);
                                        afficheur(3, secondes & 0b00001111);
                                        afficheur(2, (secondes & 0b11110000)>>4);
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
                    afficheur(2, 0);
                    afficheur(3, 0);
                    break;
                    
                case 3 :    // Commande Date
                    while(PORTAbits.RA2 != 0)
                    {
                        compteur = get_count(compteur)%5;
                        afficheur(1,compteur+1);
                        if(PORTAbits.RA1 == 0)
                        {
                            afficheur(0,0);
                            afficheur(1,0);
                            switch(compteur)
                            {
                                case 0:
                                    mois=get_number_limit(12);
                                    write_rtc(5,mois);
                                    break;
                                case 1:
                                    jours=get_number_limit(31);
                                    write_rtc(4,jours);
                                    break;
                                case 2:
                                    heures=get_number_limit(24);
                                    write_rtc(2,heures);
                                    break;
                                case 3:
                                    minutes=get_number_limit(59);
                                    write_rtc(1,minutes);
                                    break;
                                case 4:
                                    secondes=get_number_limit(60);
                                    write_rtc(0,secondes);
                                    break;
                            }
                            afficheur(0,nb_position+1);
                            afficheur(1,compteur+1);
                            afficheur(2,0);
                            afficheur(3,0);
                            __delay_ms(400);
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
                                        case_eeprom=get_number_limit(35);
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
       afficheur(2,0); 
       afficheur(3,0); 
    }         
}