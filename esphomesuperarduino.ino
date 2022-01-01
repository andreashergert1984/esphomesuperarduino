#include <EEPROM.h>
#include <Wire.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

// Debugging switches and macros
#define DEBUG 1 // Switch debug output on and off by 1 or 0

#if DEBUG
#define PRINTS(s)                                                              \
  { Serial.print(F(s)); }
#define PRINT(s, v)                                                            \
  {                                                                            \
    Serial.print(F(s));                                                        \
    Serial.print(v);                                                           \
  }
#define PRINTX(s, v)                                                           \
  {                                                                            \
    Serial.print(F(s));                                                        \
    Serial.print(F("0x"));                                                     \
    Serial.print(v, HEX);                                                      \
  }
#else
#define PRINTS(s)
#define PRINT(s, v)
#define PRINTX(s, v)
#endif

#define EEPROMMAX 512
#define MEGA2560 1
#define version 1 // current version of software and interface

// define storage areas in EEPROM
#define EEPROM_I2CADDRESS 0x00 // 1 byte
#define EEPROM_PWMCONFIG 0x01  // 6 * 6 bytes
#define EEPROM_PCCONFIG 0x25   // 24 bytes
#define EEPROM_ADCONFIG 0x3d   // 2 bytes
#define EEPROM_IOCONFIG 0x3f   // 24 bytes

#define mctype MEGA2560;           // mega2560
uint8_t i2caddress = 0x0a;         // default address
uint8_t pwmconfig[6][6] = {0};     // six bytes for six pwm configurations
uint8_t pinchangeconfig[24] = {0}; // 24 possible pin change detections
uint8_t adconfig[2] = {0};     // 2 bytes for the 16 ad channel to be enabled
uint8_t ioconfig[12][2] = {0}; // 12 Ports: first byte: input/output for pin,
                               // second byte: pullup enabled
int main() {

#if DEBUG
  Serial.begin(115200);
#endif

  void loadFromEEPROM();
  void setupAll();

  while (1) {
  }
}

void setupAll() {
  // i2c
  Wire.begin(i2caddress);       // join i2c bus with address #4
//  Wire.onReceive(receiveEvent); // register event
//  Wire.onRequest(requestEvent); // register event

  // setup PWM Output

  // setup PinChange Input

  // setup AD Input

  // setup IO 

}

void loadFromEEPROM() {
  byte initialcheck = EEPROM.read(EEPROM_I2CADDRESS);
  if (initialcheck == 255) {
    PRINTS("no init from EEPROM");
    PRINTS("initialize EEPROM");
    EEPROM.write(EEPROM_I2CADDRESS, i2caddress);
    for (uint16_t eeprompos = 1; eeprompos < EEPROMMAX; eeprompos++) {
      EEPROM.write(eeprompos, 0);
    }
    //        return;
  } // EEPROM never writen

  // i2c
  i2caddress = EEPROM.read(EEPROM_I2CADDRESS); // read i2c address
  // pwm config
  for (uint8_t pwmcounter = 0; pwmcounter < 6; pwmcounter++) {
    for (uint8_t configbyte = 0; configbyte < 6; configbyte++) {
      pwmconfig[pwmcounter][configbyte] =
          EEPROM.read(EEPROM_PWMCONFIG + pwmcounter * 6 + configbyte);
    }
  }
  // pinchange config
  for (uint8_t pcconfigcounter = 0; pcconfigcounter < 24; pcconfigcounter++) {
    pinchangeconfig[pcconfigcounter] =
        EEPROM.read(EEPROM_PWMCONFIG + pcconfigcounter);
  }
  // ad config
  adconfig[0] = EEPROM.read(EEPROM_ADCONFIG);
  adconfig[1] = EEPROM.read(EEPROM_ADCONFIG + 1);
  // digital i/o
  for (uint8_t ioconfigcounter = 0; ioconfigcounter < 12; ioconfigcounter++) {
    ioconfig[ioconfigcounter][0] =
        EEPROM.read(EEPROM_IOCONFIG + ioconfigcounter * 2);
    ioconfig[ioconfigcounter][1] =
        EEPROM.read(EEPROM_IOCONFIG + ioconfigcounter * 2 + 1);
  }
}
