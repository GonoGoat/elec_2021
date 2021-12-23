/*
 * File:   main.c
 * Author: damien & mauro
 *
 * Created on November 24, 2021, 2:23 PM
 */

// CONFIG ------------------------------------------------------------------------------------------------------------------------------------------------

#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Progr
#include <xc.h>
#define _XTAL_FREQ 16000000

#define PCF1_W 0b01000010   // Adresse en ?criture du PCF8574 1amming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF. --------------------------------------------------------------------------------------------------


#define PCF2_W 0b01000110   // Adresse en ?criture du PCF8574 2
#define PCF3_W 0b01000000   // Adresse en ?criture du PCF8574 3
#define PCF4_W 0b01001110   // Adresse en ?criture du PCF8574 4

void affiche(char nombre);
void afficheur(char position, char valeur);
void enregistrer_mesure(char measurement_data[7]);
char * lire_mesure(char position_mesure);
char get_nombre_enregistrement();
void set_nombre_enregistrement(char nombre_enregistrement);
void EEPROM_Write(char address, char data);
char EEPROM_Read(char address);
void initialisation_compteur();

void main(void) {
    
    initialisation_compteur();
    
    // Exemple de données à enregistrer
    char mesures[7]= {12, 23, 14, 30, 20, 6, 7};
       
    // initialisations   
    TRISA = 0b11111111;     // TRISA = 255 or TRISA = 0xFF
    TRISB = 0b00000000;
    TRISC = 255;
    ADCON1 = 0b00000110;    // config en I/O digitales pour toutes les pattes disponibles
    PORTB = 0b00100000;
    
    SSPSTAT = 0b11000000;
    SSPCON = 0b00101000;
    SSPCON2 = 0b00000000;
    SSPADD = 39;            // Voir datasheet page 82, bit 3-0

    enregistrer_mesure(mesures);
    
   
    
    // Lecture de données --------------------------
    // Début de la procédure de lecture
    char numero_enregistrement = 1;
    char *l = lire_mesure(numero_enregistrement);
    char data[7]; // Tableau à manipuler
    
    for (char i = 0; i < 8; i ++){
        data[i] = *(l+i);
    }
    // Fin de la procédure de lecture---------------
    
    
    while(1){   
        affiche(data[0]);
    }
    return;
}


// Afficher un nombre dans LES 7segments
void affiche(char nombre){ 
    afficheur(3, nombre % 10);  // Unite
    afficheur(2, nombre / 10);  // Dizaine
    afficheur(1, nombre / 100);
    afficheur(0, nombre / 1000); 
}


// Afficher une valeur dans un 7segments
void afficheur(char position, char valeur){
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


void enregistrer_mesure(char measurement_data[7]){
    char nombre_enregistrement = get_nombre_enregistrement();          
    char position;  // addresse de l'eeprom

    if (nombre_enregistrement < 36){
        position = (nombre_enregistrement * 7) + 1;
    }
    else { // (nombre_enregistrement >= 36)
        position = 1;
        nombre_enregistrement = 0;
    }

    // Ecriture des donnï¿½es
    for (char i = 0; i < 7; i++) {
        EEPROM_Write(position, measurement_data[i]);
        position++;
    }

    // Mise ï¿½ jour du nombre d'enregistrements
    set_nombre_enregistrement(nombre_enregistrement);
}

// Lire une mesure
char * lire_mesure(char position_mesure){
    static char data[7]; // Tableau des données lues
    char case_eeprom = (position_mesure * 7) + 1;
    
    for(char i = 0; i < 7; i++){
        data[i] = EEPROM_Read(case_eeprom + i); 
    }
    
    return data;
}

// retourne le nombre d'enregistrement 
char get_nombre_enregistrement(){
    return EEPROM_Read(0);
}

// definir le nombre d'enregistrement
void set_nombre_enregistrement(char nombre_enregistrement){
    nombre_enregistrement += 1;
    EEPROM_Write(0, nombre_enregistrement);
}

// ecrire une donnée dans une case de l'eeprom
void EEPROM_Write(char address, char data){
    EEADR = address;                    // Write address to the EEADR register 
    EEDATA = data;                      // Copy data to the EEDATA register for write to EEPROM location 
    EECON1bits.EEPGD = 0;               // Access data EEPROM memory
    EECON1bits.WRERR = 0;               // EEPROM Error Flag bit
    EECON1bits.WREN = 1;                // Allow write to the memory
    INTCONbits.GIE = 0;                 //  Disable global interrupt
    
    // Assign below sequence to EECON2 Register is necessary to write data to EEPROM memory 
    EECON2 = 0x55;
    EECON2 = 0xaa;
    
    EECON1bits.WR = 1;                  // Start writing data to EEPROM memory
    INTCONbits.GIE = 1;                 // Enable interrupt
    
    while(PIR2bits.EEIF == 0);          // Wait for write operation complete
    PIR2bits.EEIF = 0;                  // Reset EEIF for further write operation
}

// lire une donnée d'une case de l'eeprom
char EEPROM_Read(char address){
    EEADR = address;                    // Read address to the EEADR register 
    EECON1bits.WREN = 0;                // Allow read to the memory
    EECON1bits.EEPGD = 0;               // Access data EEPROM memory
    EECON1bits.RD = 1;                  // Initiates an EEPROM read
    return EEDATA;
}

// initialiser le compteur, nécessaire en cas de mise hors tension
void initialisation_compteur(){
    char valeur_actuelle = EEPROM_Read(0);
    if(!valeur_actuelle >= 0 || !valeur_actuelle <=36){
        EEPROM_Write(0, 0); // Initionalisation Ã  0
    }
}