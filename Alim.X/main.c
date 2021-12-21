/* 
 * File:   main.c
 * Author: thomashamamji
 *
 * Created on 21 décembre 2021, 12:40
 */

#include "main.h"

int ascii_hex_to_decimal(char car) {
    int num = 0;
    num = num << 4;
    num = (car<='9')?(car-'0'):(car-'a'+10);
    return num;
}

void decimal_to_ascii_hex(int decimal, char * buffer) {
    sprintf(buffer, "%x", decimal);
}

int chksum_calculation (const char * chain, char * buffer) {
  int i, j, chksum = 0;
  char ascii[16] = "0123456789abcdef";
  char chksum_str[3] = "";

  strcpy(buffer, chain);
  
  for (i = 0; chain[i]; i+=2) {
    int s = ascii_hex_to_decimal(chain[i]) * 16 + ascii_hex_to_decimal(chain[i+1]);
    chksum += s;
  }

  printf("%d %x\n", chksum, chksum);
  chksum = (chksum & 0b11111111);

  decimal_to_ascii_hex(256-chksum, chksum_str);
  strcat(buffer, chksum_str);

  return chksum;
}

void control_alim (char * tram) {
    char new_tram[30] = "";
    chksum_calculation(tram, new_tram);

    // UART(new_tram) -> REPONSE(new_tram)
}

void set_voltage (char * tram) {
  char new_tram[30] = "";

  // Set voltage to tram

  chksum_calculation(tram, new_tram);
}

int main(void) {
    char str_chks[40] = "";
    printf("%d\n", chksum_calculation("010300200002", str_chks));
    printf("%s\n", str_chks);
    
    return (0);
}