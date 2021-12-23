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

int main(void) {
    // set_voltage(10); // 10 V
    // set_current(10); // 10 A

    // Put the config in the main !!!

    RCSTA = RCSTA_byte;
    TXSTA = TXSTA_byte;
    SPBRG = SPBRG_byte;

    set_power(35); // 35 W
    return (0);
}