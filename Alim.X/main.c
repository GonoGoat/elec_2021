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
#define _XTAL_FREQ 10000000

#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#define PCF1_W 0b01000010   // Adresse en ?criture du PCF8574 1
#define PCF2_W 0b01000110   // Adresse en ?criture du PCF8574 2
#define PCF3_W 0b01000000   // Adresse en ?criture du PCF8574 3
#define PCF4_W 0b01001110   // Adresse en ?criture du PCF8574 4

#include "main.h"

// PREFIXES
#define PRE_SET_VALUE ":01060015"

// ENABLE
#define ENABLE_REMOTE_MODE ":010601000001F7"

// CONFIGURE 
#define ENABLE_VOLTAGE ":010600160000E3"
#define ENABLE_CURRENT ":010600160010D3"
#define ENABLE_POWER ":010600160020C3"

// READING
#define READ_VOLTAGE ":010300100001EB"
#define READ_CURRENT ":010300110001EA"
#define READ_POWER ":010300120001E9"

// COMMANDS
#define POWER_ON ":010600170011D1"
#define POWER_OFF ":010600170000E2"


/* PORTAbits.RA0 = Bouton Up
* PORTAbits.RA1 = Bouton OK
* PORTAbits.RA2 = Bouton Back
* 
* PORTBbits.RB5 = LED_1
* PORTBbits.RB4 = LED_2
*/

void to_upper (char * str) {
  for (int i = 0; str[i]; i++) {
    char c = str[i];
    if (c >= 97 && c <= 122) {
      c -= 32;
    }
    str[i] = c;
  }
}

char ascii_hex_to_decimal(char letter) {
    char num = 0;
    num = num << 4;
    num = (letter<='9')?(letter-'0'):(letter-'a'+10);
    num = (num & 0b00001111);
    return num;
}

void decimal_to_ascii_hex(int decimal, char * buffer) {
    sprintf(buffer, "%x", decimal);
}

void send_tram(char * tram) {
    while(*tram != '\0') {
        while (!TXIF);
        TXREG = *tram;
        tram++;
    }
    while (!TXIF);
    TXREG = '\r';
    while (!TXIF);
    TXREG = '\n';
}

void receive_tram(char * buffer) { 
    RCSTAbits.CREN = 1;
    if(OERR) {
        //If error, set bit of reading to 1.
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
    }
    
    while(!RCIF);
    while(RCREG != '\r') {
        //Wait until the reception buffer is empty.
        while(!RCIF);
        *buffer = RCREG;
        buffer++;
    }
}

int chksum_calculation (const char * chain, char * buffer) {
  int i, j, chksum = 0;
  char chksum_str[3] = "";

  strcpy(buffer, chain);
  
  for (i = 1; chain[i]; i+=2) {
    int s = ascii_hex_to_decimal(chain[i]) * 16 + ascii_hex_to_decimal(chain[i+1]);
    chksum += s;
  }

  printf("%d %x\n", chksum, chksum);
  chksum = (chksum & 0b11111111);

  decimal_to_ascii_hex(256-chksum, chksum_str);
  strcat(buffer, chksum_str);

  return chksum;
}

void set_value (char tram[30], int val) {
    char asciihex[10] = "";
    sprintf(asciihex, "%04x", val);
    strcat(tram, asciihex);
}

void set_voltage (int voltage) {
    // Enable remote mode
    // Send UART
    send_tram(ENABLE_REMOTE_MODE);
    // Enable voltage mode
    // Send UART
    send_tram(ENABLE_VOLTAGE);
    // Set V value
    char new_tram3[30] = "";
    char tram[30] = PRE_SET_VALUE;
    set_value(tram, voltage);
    // Checksum
    chksum_calculation(tram, new_tram3);
    to_upper(new_tram3);
    // Send UART
    send_tram(new_tram3);
    printf("%s\n%s\n%s\n", ENABLE_REMOTE_MODE, ENABLE_VOLTAGE, new_tram3);
}

void set_current (int current) {
    // Enable remote mode
    // Send UART
    send_tram(ENABLE_REMOTE_MODE);
    // Enable current mode
    // Send UART
    send_tram(ENABLE_CURRENT);
    // Set A value
    char new_tram3[30] = "";
    char tram[30] = PRE_SET_VALUE;
    set_value(tram, current);
    // Checksum
    chksum_calculation(tram, new_tram3);
    to_upper(new_tram3);
    // Send UART
    send_tram(new_tram3);
    printf("%s\n%s\n%s\n", ENABLE_REMOTE_MODE, ENABLE_CURRENT, new_tram3);
}

void set_power (int power) {
    // Enable remote mode
    // Send UART
    send_tram(ENABLE_REMOTE_MODE);
    // Enable power mode
    // Send UART
    send_tram(ENABLE_POWER);
    // Set W value
    char new_tram3[30] = "";
    char tram[30] = PRE_SET_VALUE;
    set_value(tram, power);
    // Checksum
    chksum_calculation(tram, new_tram3);
    to_upper(new_tram3);
    // Send UART
    send_tram(new_tram3);
    printf("%s\n%s\n%s\n", ENABLE_REMOTE_MODE, ENABLE_POWER, new_tram3);
}

void turn_power_on (void) {
  send_tram(POWER_ON);
}

void turn_power_off (void) {
  send_tram(POWER_OFF);
}

char get_val() {
    char result = 0;
    char buffer[30];
    receive_tram(buffer);
    for(char i = 7; i<11; i++) {
        result += ascii_hex_to_decimal(buffer[i]);
        if(i!=10) result = result << 4; 
    }
    return result;
}

char get_voltage () {
    send_tram(READ_VOLTAGE);
    return get_val();
}

char get_current () {
    send_tram(READ_CURRENT);
    return get_val();
}

char get_power () {
    send_tram(READ_POWER);
    return get_val();
}

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

void EEPROM_Write(int address, int data){
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

int EEPROM_Read(int address){
    EEADR = address;                    // Read address to the EEADR register 
    EECON1bits.WREN = 0;                // Allow read to the memory
    EECON1bits.EEPGD = 0;               // Access data EEPROM memory
    EECON1bits.RD = 1;                  // Initiates an EEPROM read
    return EEDATA;
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
    
    RCSTA = RCSTA_byte;
    TXSTA = TXSTA_byte;
    SPBRG = SPBRG_byte;

    set_power(35); // 35 W
    
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
        PORTBbits.RB4 = !PORTBbits.RB4;
        PORTBbits.RB5 = compteur_ON_OFF;
        __delay_ms(400);
        
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
                                            // faire le changement : Allumer ou �teindre
                                        }    
                                        break;
                                        
                                    case 1 :        //Commande Tension
                                        compteur2 = 0;
                                        compteur3 = 0;
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
                                                    compteur2 = get_count(compteur2)%10; // Obtiens l'unit� de la tension voulue
                                                    if(PORTAbits.RA1 == 0)
                                                    {
                                                        //Definir tention = compteur2*10+compteur2
                                                         fin = 1;
                                                        __delay_ms(400); 
                                                    }
                                                }                          
                                            }
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
                                        // Selection de la case m�moire � atteindre
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
                                        // Lecture sur l'EEPROM de l'intensit�
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