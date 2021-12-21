#include "main.h"

// PREFIXES
#define PRE_SET_VALUE ":01060015"

// ENABLE
#define ENABLE_REMOTE_MODE ":010601000001f7"

// CONFIGURE 
#define ENABLE_VOLTAGE ":010600160000e3"
#define ENABLE_CURRENT ":010600160010d3"
#define ENABLE_POWER ":010600160020c3"

// COMMANDS
#define POWER_ON ":010600170011D1"
#define POWER_OFF ":010600170000E2"

/*

    Missing :
        *   ENABLE_CURRENT
        *   ENABLE_POWER

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
    // Enable voltage mode
    // Send UART
    // Set V value
    char new_tram3[30] = "";
    char tram[30] = PRE_SET_VALUE;
    set_value(tram, voltage);
    // Checksum
    chksum_calculation(tram, new_tram3);
    to_upper(new_tram3);
    // Send UART
    printf("%s\n%s\n%s\n", ENABLE_REMOTE_MODE, ENABLE_VOLTAGE, new_tram3);
}

void set_current (int current) {
    // Enable remote mode
    // Send UART
    // Enable current mode
    // Send UART
    // Set A value
    char new_tram3[30] = "";
    char tram[30] = PRE_SET_VALUE;
    set_value(tram, current);
    // Checksum
    chksum_calculation(tram, new_tram3);
    to_upper(new_tram3);
    // Send UART
    printf("%s\n%s\n%s\n", ENABLE_REMOTE_MODE, ENABLE_CURRENT, new_tram3);
}

void set_power (int power) {
    // Enable remote mode
    // Send UART
    // Enable power mode
    // Send UART
    // Set W value
    char new_tram3[30] = "";
    char tram[30] = PRE_SET_VALUE;
    set_value(tram, power);
    // Checksum
    chksum_calculation(tram, new_tram3);
    to_upper(new_tram3);
    // Send UART
    printf("%s\n%s\n%s\n", ENABLE_REMOTE_MODE, ENABLE_POWER, new_tram3);
}

int main(void) {
    set_voltage(10); // 10 V
    set_current(10); // 10 A
    set_power(35); // 10 W
    return (0);
}