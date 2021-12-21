/* 
 * File:   main.c
 * Author: thomashamamji
 *
 * Created on 21 décembre 2021, 12:40
 */

#include "main.h"

// Trames de lecture.
// Trames d'ecriture.
#define ENABLE_VOLTAGE ":010600160000e3"
#define ENABLE_REMOTE ":010601000001f7"

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
      TXREG = *tram;
      RCSTAbits.CREN = 0;
      //printf("%c", *tram);
      tram++;
    }
    TXREG = "\r";
    RCSTAbits.CREN = 0;
    TXREG = "\n";
    RCSTAbits.CREN = 0;
}

int chksum_calculation (const char * chain, char * buffer) {
  int i, j, chksum = 0;
  char ascii[16] = "0123456789abcdef";
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

void set_value (char * tram, int val) {
    char asciihex[10] = "";
    sprintf(asciihex, "%04x", val);
    strcat(tram, asciihex);
}

void set_voltage (int voltage) {
    // Enable remote mode
    // Send UART
    // Enable voltage mode
    // Send UART
    char tram3[30] = ":01060015";
    // Set 10 V
    char new_tram3[30] = "";
    set_value(tram3, voltage);
    // Checksum
    chksum_calculation(tram3, new_tram3);
    // Send UART
    printf("%s\n", new_tram3);
}

char get_voltage () {
  // TODO
  return 0;
}

char get_current () {
  // TODO
  return 0;
}

char get_power () {
  // TODO
  return 0;
}

int main(void) {
    set_voltage(10); // 10 V
    return (0);
}