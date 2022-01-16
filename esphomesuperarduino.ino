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
#define VERSION 1 // current version of software and interface

// define storage areas in EEPROM
#define EEPROM_I2CADDRESS 0x00 // 1 byte
#define EEPROM_PWMCONFIG 0x01  // 6 * 6 bytes
#define EEPROM_PCCONFIG 0x25   // 24 bytes
#define EEPROM_ADCONFIG 0x3d   // 2 bytes
#define EEPROM_IOCONFIG 0x3f   // 24 bytes

#define MCTYPE MEGA2560;       // mega2560
uint8_t i2caddress = 0x0a;     // default address
uint8_t pwmconfig[6][6] = {0}; // six bytes for six pwm configurations
uint16_t pwmduty[18] = {
    0}; // current requested pwm duty (always stored as 16 bit)
uint16_t rpmrequest[18] = {
    0}; // current requested rpm (always stored as 16 bit)
uint8_t pinchangeconfig[24] = {0}; // 24 possible pin change detections
uint8_t adconfig[2] = {0};     // 2 bytes for the 16 ad channel to be enabled
uint8_t ioconfig[12][2] = {0}; // 12 Ports: first byte: input/output for pin,
                               // second byte: pullup enabled
uint8_t usedpins[12] = {0};    // an array to store all used pins on setupAll to
                               // prevent configuring the same pin twice
uint16_t pccounter[12];        // counter for pin changes
uint8_t
    pcintstate[3]; // save the last state to determine which input was changed
uint8_t pcintrisingmask[3] = {0};
uint8_t pcintfallingmask[3] = {0};
uint16_t currentanalogvalue[15] = {0};
uint8_t currentanalogcounter = 0;
uint8_t req_register; // requested Byte

int main() {

#if DEBUG
  usedpins[4] |= 3; // PE0 and PE1 for uart
  Serial.begin(115200);
#endif

  void loadFromEEPROM();
  void setupAll();
  while (1) {
  }
}

void setupAll() {
  // i2c
  Wire.begin(i2caddress);       // join i2c bus as slave
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event

  // setup PWM Output
  setupPWM0();
  setupPWM1();
  setupPWM2();
  setupPWM3();
  setupPWM4();
  setupPWM5();
  // setup PinChange Input
  setupPC();
  // TODO RPM calculation
  //  setup AD Input
  setupADC();

  // setup IO
  setupIO();
  sei();
}

void requestEvent() {
  PRINTX("i2c read for ", req_register);
  byte responsedata[6];
  switch (req_register) {
  case 0x00: // version
    responsedata[0] = VERSION;
    Wire.write(responsedata[0]);
    break;
  case 0x01: // controller type
    responsedata[0] = MCTYPE;
    Wire.write(responsedata[0]);
    break;
  case 0x10: // PWM Config
  case 0x11: // PWM Config
  case 0x12: // PWM Config
  case 0x13: // PWM Config
  case 0x14: // PWM Config
  case 0x15: // PWM Config
    for (auto configbyte = 0; configbyte < 6; configbyte++) {
      responsedata[configbyte] = EEPROM.read(
          EEPROM_PWMCONFIG + (req_register - 0x10) * 6 + configbyte);
    }
    Wire.write(responsedata, 6);
    break;
  case 0x30: // pwm duty 1byte
  case 0x31: // pwm duty 1byte
  case 0x32: // pwm duty 1byte
  case 0x33: // pwm duty 1byte
  case 0x34: // pwm duty 1byte
  case 0x35: // pwm duty 1byte
  case 0x36: // pwm duty 1byte
  case 0x37: // pwm duty 1byte
  case 0x38: // pwm duty 1byte
  case 0x39: // pwm duty 1byte
  case 0x3a: // pwm duty 1byte
  case 0x3b: // pwm duty 1byte
  case 0x3c: // pwm duty 1byte
  case 0x3d: // pwm duty 1byte
  case 0x3e: // pwm duty 1byte
  case 0x3f: // pwm duty 1byte
  case 0x40: // pwm duty 1byte
  case 0x41: // pwm duty 1byte
    responsedata[0] = pwmduty[req_register - 0x30] / 256;
    Wire.write(responsedata[0]);
    break;
  case 0x50: // pwm duty 2bytes
  case 0x51: // pwm duty 2bytes
  case 0x52: // pwm duty 2bytes
  case 0x53: // pwm duty 2bytes
  case 0x54: // pwm duty 2bytes
  case 0x55: // pwm duty 2bytes
  case 0x56: // pwm duty 2bytes
  case 0x57: // pwm duty 2bytes
  case 0x58: // pwm duty 2bytes
  case 0x59: // pwm duty 2bytes
  case 0x5a: // pwm duty 2bytes
  case 0x5b: // pwm duty 2bytes
  case 0x5c: // pwm duty 2bytes
  case 0x5d: // pwm duty 2bytes
  case 0x5e: // pwm duty 2bytes
  case 0x5f: // pwm duty 2bytes
  case 0x60: // pwm duty 2bytes
  case 0x61: // pwm duty 2bytes
    responsedata[0] = pwmduty[req_register - 0x30] / 256;
    responsedata[1] = pwmduty[req_register - 0x30] % 256;
    Wire.write(responsedata,2);
    break;
  case 0x70: // RPM 2 bytes
  case 0x71: // RPM 2 bytes
  case 0x72: // RPM 2 bytes
  case 0x73: // RPM 2 bytes
  case 0x74: // RPM 2 bytes
  case 0x75: // RPM 2 bytes
  case 0x76: // RPM 2 bytes
  case 0x77: // RPM 2 bytes
  case 0x78: // RPM 2 bytes
  case 0x79: // RPM 2 bytes
  case 0x7a: // RPM 2 bytes
  case 0x7b: // RPM 2 bytes
  case 0x7c: // RPM 2 bytes
  case 0x7d: // RPM 2 bytes
  case 0x7e: // RPM 2 bytes
  case 0x7f: // RPM 2 bytes
  case 0x80: // RPM 2 bytes
  case 0x81: // RPM 2 bytes
    responsedata[0] = pwmduty[req_register - 0x70] / 256;
    responsedata[1] = pwmduty[req_register - 0x70] % 256;
    Wire.write(responsedata,2);
    break;
  case 0x90: // pinchange config 1 byte
  case 0x91: // pinchange config 1 byte
  case 0x92: // pinchange config 1 byte
  case 0x93: // pinchange config 1 byte
  case 0x94: // pinchange config 1 byte
  case 0x95: // pinchange config 1 byte
  case 0x96: // pinchange config 1 byte
  case 0x97: // pinchange config 1 byte
  case 0x98: // pinchange config 1 byte
  case 0x99: // pinchange config 1 byte
  case 0x9a: // pinchange config 1 byte
  case 0x9b: // pinchange config 1 byte
  case 0x9c: // pinchange config 1 byte
  case 0x9d: // pinchange config 1 byte
  case 0x9e: // pinchange config 1 byte
  case 0x9f: // pinchange config 1 byte
  case 0xa0: // pinchange config 1 byte
  case 0xa1: // pinchange config 1 byte
  case 0xa2: // pinchange config 1 byte
  case 0xa3: // pinchange config 1 byte
  case 0xa4: // pinchange config 1 byte
  case 0xa5: // pinchange config 1 byte
  case 0xa6: // pinchange config 1 byte
  case 0xa7: // pinchange config 1 byte
    responsedata[0] = EEPROM.read(EEPROM_PCCONFIG + (req_register - 0x90));
    Wire.write(responsedata[0]);
    break;
  case 0xb0: // adc config 1 byte
  case 0xb1: // adc config 1 byte
    responsedata[0] = EEPROM.read(EEPROM_ADCONFIG + (req_register - 0xb0));
    Wire.write(responsedata[0]);
    break;
  case 0xc0: // config digital io porta 2 bytes
  case 0xc1: // config digital io portb 2 bytes
  case 0xc2: // config digital io portc 2 bytes
  case 0xc3: // config digital io portd 2 bytes
  case 0xc4: // config digital io porte 2 bytes
  case 0xc5: // config digital io portf 2 bytes
  case 0xc6: // config digital io portg 2 bytes
  case 0xc7: // config digital io porth 2 bytes
  case 0xc8: // config digital io porti 2 bytes
  case 0xc9: // config digital io portj 2 bytes
  case 0xca: // config digital io portk 2 bytes
  case 0xcb: // config digital io portl 2 bytes
    for (auto configbyte = 0; configbyte < 2; configbyte++) {
      responsedata[configbyte] =
          EEPROM.read(EEPROM_IOCONFIG + (req_register - 0xc0) * 2 + configbyte);
    }
    Wire.write(responsedata, 2);
    break;
  }
}

void receiveEvent(int howMany) {

  if (howMany == 0) {
    PRINTS("just a ping");
    return; // just a ping
  }
  req_register = Wire.read();
  bool changed = false;
  uint8_t data[6]; // up to 6 bytes data is defined
  switch (req_register) {
  case 0x10: // PWM Config
  case 0x11: // PWM Config
  case 0x12: // PWM Config
  case 0x13: // PWM Config
  case 0x14: // PWM Config
  case 0x15: // PWM Config
    if (howMany == 7) {
      for (auto i = 0; i < 6; i++) {
        data[i] = Wire.read();
      }
      for (auto configbyte = 0; configbyte < 6; configbyte++) {
        if (EEPROM.read(EEPROM_PWMCONFIG + (req_register - 0x10) * 6 +
                        configbyte) != data[configbyte]) {
          changed = true;
        }
        EEPROM.update(EEPROM_PWMCONFIG + (req_register - 0x10) * 6 + configbyte,
                      data[configbyte]);
      }

    } else {
      PRINTS("got other than 6 byte data for pwm config");
    }
    break;
  case 0x30: // pwm duty 1byte
  case 0x31: // pwm duty 1byte
  case 0x32: // pwm duty 1byte
  case 0x33: // pwm duty 1byte
  case 0x34: // pwm duty 1byte
  case 0x35: // pwm duty 1byte
  case 0x36: // pwm duty 1byte
  case 0x37: // pwm duty 1byte
  case 0x38: // pwm duty 1byte
  case 0x39: // pwm duty 1byte
  case 0x3a: // pwm duty 1byte
  case 0x3b: // pwm duty 1byte
  case 0x3c: // pwm duty 1byte
  case 0x3d: // pwm duty 1byte
  case 0x3e: // pwm duty 1byte
  case 0x3f: // pwm duty 1byte
  case 0x40: // pwm duty 1byte
  case 0x41: // pwm duty 1byte
    // TODO
    break;
  case 0x50: // pwm duty 2bytes
  case 0x51: // pwm duty 2bytes
  case 0x52: // pwm duty 2bytes
  case 0x53: // pwm duty 2bytes
  case 0x54: // pwm duty 2bytes
  case 0x55: // pwm duty 2bytes
  case 0x56: // pwm duty 2bytes
  case 0x57: // pwm duty 2bytes
  case 0x58: // pwm duty 2bytes
  case 0x59: // pwm duty 2bytes
  case 0x5a: // pwm duty 2bytes
  case 0x5b: // pwm duty 2bytes
  case 0x5c: // pwm duty 2bytes
  case 0x5d: // pwm duty 2bytes
  case 0x5e: // pwm duty 2bytes
  case 0x5f: // pwm duty 2bytes
  case 0x60: // pwm duty 2bytes
  case 0x61: // pwm duty 2bytes
    // TODO
    break;
  case 0x70: // RPM 2 bytes
  case 0x71: // RPM 2 bytes
  case 0x72: // RPM 2 bytes
  case 0x73: // RPM 2 bytes
  case 0x74: // RPM 2 bytes
  case 0x75: // RPM 2 bytes
  case 0x76: // RPM 2 bytes
  case 0x77: // RPM 2 bytes
  case 0x78: // RPM 2 bytes
  case 0x79: // RPM 2 bytes
  case 0x7a: // RPM 2 bytes
  case 0x7b: // RPM 2 bytes
  case 0x7c: // RPM 2 bytes
  case 0x7d: // RPM 2 bytes
  case 0x7e: // RPM 2 bytes
  case 0x7f: // RPM 2 bytes
  case 0x80: // RPM 2 bytes
  case 0x81: // RPM 2 bytes
    // TODO
    break;
  case 0x90: // pinchange config 1 byte
  case 0x91: // pinchange config 1 byte
  case 0x92: // pinchange config 1 byte
  case 0x93: // pinchange config 1 byte
  case 0x94: // pinchange config 1 byte
  case 0x95: // pinchange config 1 byte
  case 0x96: // pinchange config 1 byte
  case 0x97: // pinchange config 1 byte
  case 0x98: // pinchange config 1 byte
  case 0x99: // pinchange config 1 byte
  case 0x9a: // pinchange config 1 byte
  case 0x9b: // pinchange config 1 byte
  case 0x9c: // pinchange config 1 byte
  case 0x9d: // pinchange config 1 byte
  case 0x9e: // pinchange config 1 byte
  case 0x9f: // pinchange config 1 byte
  case 0xa0: // pinchange config 1 byte
  case 0xa1: // pinchange config 1 byte
  case 0xa2: // pinchange config 1 byte
  case 0xa3: // pinchange config 1 byte
  case 0xa4: // pinchange config 1 byte
  case 0xa5: // pinchange config 1 byte
  case 0xa6: // pinchange config 1 byte
  case 0xa7: // pinchange config 1 byte
    if (howMany == 2) {
      data[0] = Wire.read();
      if (EEPROM.read(EEPROM_PCCONFIG + (req_register - 0x90)) != data[0]) {
        changed = true;
      }
      EEPROM.update(EEPROM_PCCONFIG + (req_register - 0x90), data[0]);
    } else {
      PRINTS("got other than 1 byte data for pinchange config");
    }
    break;
  case 0xb0: // adc config 1 byte
  case 0xb1: // adc config 1 byte
    if (howMany == 2) {
      data[0] = Wire.read();
      if (EEPROM.read(EEPROM_ADCONFIG + (req_register - 0xb0)) != data[0]) {
        changed = true;
      }

      EEPROM.update(EEPROM_ADCONFIG + (req_register - 0xb0), data[0]);
    } else {
      PRINTS("got other than 1 byte data for adc config");
    }
    break;
  case 0xc0: // digital io porta 2 bytes
  case 0xc1: // digital io portb 2 bytes
  case 0xc2: // digital io portc 2 bytes
  case 0xc3: // digital io portd 2 bytes
  case 0xc4: // digital io porte 2 bytes
  case 0xc5: // digital io portf 2 bytes
  case 0xc6: // digital io portg 2 bytes
  case 0xc7: // digital io porth 2 bytes
  case 0xc8: // digital io porti 2 bytes
  case 0xc9: // digital io portj 2 bytes
  case 0xca: // digital io portk 2 bytes
  case 0xcb: // digital io portl 2 bytes
    if (howMany == 3) {
      for (auto i = 0; i < 1; i++) {
        data[i] = Wire.read();
      }
      for (auto configbyte = 0; configbyte < 2; configbyte++) {
        if (EEPROM.read(EEPROM_IOCONFIG + (req_register - 0xc0) * 2 +
                        configbyte) != data[configbyte]) {
          changed = true;
        }

        EEPROM.update(EEPROM_IOCONFIG + (req_register - 0xc0) * 2 + configbyte,
                      data[configbyte]);
      }
    } else {
      PRINTS("got other than 2 byte data for io config");
    }
    break;
  }
  while (Wire.available()) {
    Wire.read();
  };
  if (changed) {
    loadFromEEPROM();
    setupAll();
  }
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
        EEPROM.read(EEPROM_PCCONFIG + pcconfigcounter);
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

void setupIO() {
  //  ioconfig[12]
  // setting direction filtered by already used ports
  // output set
  DDRA |= ioconfig[0][0] & ~usedpins[0];
  DDRB |= ioconfig[1][0] & ~usedpins[1];
  DDRC |= ioconfig[2][0] & ~usedpins[2];
  DDRD |= ioconfig[3][0] & ~usedpins[3];
  DDRE |= ioconfig[4][0] & ~usedpins[4];
  DDRF |= ioconfig[5][0] & ~usedpins[5];
  DDRG |= ioconfig[6][0] & ~usedpins[6];
  DDRH |= ioconfig[7][0] & ~usedpins[7];
  //  DDRI |= ioconfig[8][0] & ~usedpins[8];
  DDRJ |= ioconfig[9][0] & ~usedpins[9];
  DDRK |= ioconfig[10][0] & ~usedpins[10];
  DDRL |= ioconfig[11][0] & ~usedpins[11];
  // input set
  DDRA &= ~(ioconfig[0][0] | usedpins[0]);
  DDRB &= ~(ioconfig[1][0] | usedpins[1]);
  DDRC &= ~(ioconfig[2][0] | usedpins[2]);
  DDRD &= ~(ioconfig[3][0] | usedpins[3]);
  DDRE &= ~(ioconfig[4][0] | usedpins[4]);
  DDRF &= ~(ioconfig[5][0] | usedpins[5]);
  DDRG &= ~(ioconfig[6][0] | usedpins[6]);
  DDRH &= ~(ioconfig[7][0] | usedpins[7]);
  //  DDRI &= ~(ioconfig[8][0] | usedpins[8]);
  DDRJ &= ~(ioconfig[9][0] | usedpins[9]);
  DDRK &= ~(ioconfig[10][0] | usedpins[10]);
  DDRL &= ~(ioconfig[11][0] | usedpins[11]);

  // pullups set filtered by free pins
  PORTA = ioconfig[0][1] & ~usedpins[0];
  PORTB = ioconfig[1][1] & ~usedpins[1];
  PORTC = ioconfig[2][1] & ~usedpins[2];
  PORTD = ioconfig[3][1] & ~usedpins[3];
  PORTE = ioconfig[4][1] & ~usedpins[4];
  PORTF = ioconfig[5][1] & ~usedpins[5];
  PORTG = ioconfig[6][1] & ~usedpins[6];
  PORTH = ioconfig[7][1] & ~usedpins[7];
  //  PORTI = ioconfig[8][1] & ~usedpins[8];
  PORTJ = ioconfig[9][1] & ~usedpins[9];
  PORTK = ioconfig[10][1] & ~usedpins[10];
  PORTL = ioconfig[11][1] & ~usedpins[11];
}
/* PORTF PORTK
adconfig[2]*/
void setupADC() {
  // checking used ports
  for (auto i = 0; i < 8; i++) {
    if (usedpins[6] & (1 << i)) {
      adconfig[0] &= ~(1 << i);
    }
    if (usedpins[11] & (1 << i)) {
      adconfig[1] &= ~(1 << i);
    }
  }
  // set pins as input
  for (auto i = 0; i < 8; i++) {
    if (adconfig[0] & (1 << i)) {
      DDRF &= ~(1 << i);
    }
    if (adconfig[1] & (1 << i)) {
      DDRK &= ~(1 << i);
    }
  }

  ADMUX = 0; // AREF is used
  ADCSRA = (1 << ADEN) & (1 << ADIE);

  // disable digital inputs for the analog pins
  DIDR0 = adconfig[0];
  DIDR2 = adconfig[1];
  usedpins[6] |= adconfig[0];
  usedpins[11] |= adconfig[1];
  sei();
}

/*
PCINT0: PORTB
PCINT1: PE0; PJ0-PJ6
PCINT2: PORTK

*/
void setupPC() {
  /*  uint8_t pinchangeconfig[24] = {0}; // 24 possible pin change detections
  | Byte | Bit | Description |
  | ----------- | ----------- |----------- |
  | 0 | 0 | enable/disable this pin change interrupt
  | 0 | 1 | enable internal pullup
  | 0 | 2 | inc counter on rising edge
  | 0 | 3 | inc counter on falling edge
  | 0 | 4 |
  | 0 | 5 |
  | 0 | 6 |
  | 0 | 7 |
  */
  // checking for used pins
  // for PORT PB
  for (auto i = 0; i < 8; i++) {
    if (usedpins[1] & (1 << i)) {
      pinchangeconfig[i] =
          0; // disable PinChange since already used by something
    }
  }
  // for PORT PE0
  if (usedpins[5] & (1 << 0)) {
    pinchangeconfig[8] = 0;
  }
  // for PORT PJ
  for (auto i = 9; i < 16; i++) {
    if (usedpins[10] & (1 << i)) {
      pinchangeconfig[i] =
          0; // disable PinChange since already used by something
    }
  }
  // for PORT PK
  for (auto i = 16; i < 24; i++) {
    if (usedpins[11] & (1 << i)) {
      pinchangeconfig[i] =
          0; // disable PinChange since already used by something
    }
  }

  bool pciused[3] = {false}; // which pinchange interruped will be used
  // setting up PC
  for (auto i = 0; i < 24; i++) {
    if (pinchangeconfig[i] & 1) { // pin for PC enabled
                                  // TODO usedpins
      pciused[i % 8] = 1;         // store if pci is used;
      // PORTB
      if (i < 8) {
        // input
        DDRB &= ~(1 << i);
        usedpins[1] |= (1 << i);
        // pullup enabled
        if (pinchangeconfig[i] & (1 << 1)) {
          PORTB |= (1 << i);
        }
        // enable pcintport mask
        PCMSK0 |= (1 << i);
      }
      // PORTE0
      if (i == 8) {
        // input
        DDRE &= ~(1 << DDE0);
        usedpins[5] |= (1 << DDE0);
        // pullup enabled
        if (pinchangeconfig[i] & (1 << 1)) {
          PORTE |= 1;
        }
        // enable pcintport mask
        PCMSK1 |= 1;
      }
      if (i > 8 && i < 16) {
        // input
        DDRJ &= ~(1 << (i - 9));
        usedpins[10] |= (1 << (i - 9));
        // pullup enabled
        if (pinchangeconfig[i] & (1 << 1)) {
          PORTJ |= (1 << i - 9);
        }
        // enable pcintport mask
        PCMSK1 |= (1 << i - 8);
      }
      if (i >= 16) {
        // input
        DDRK &= ~(1 << (i - 16));
        usedpins[1] |= (1 << i);
        // pullup enabled
        if (pinchangeconfig[i] & (1 << 1)) {
          PORTK |= (1 << i - 16);
        }
        // enable pcintport mask
        PCMSK2 |= (1 << i - 16);
      }
      if (pinchangeconfig[i] & (1 << 2)) {
        pcintrisingmask[(uint8_t)(i / 8)] |= (1 << i % 8);
      }
      if (pinchangeconfig[i] & (1 << 3)) {
        pcintfallingmask[(uint8_t)(i / 8)] |= (1 << i % 8);
      }
    }
  }
  // enable all used PCI
  PCICR = 0;
  if (pciused[0]) {
    PCICR |= PCIE0;
  }
  if (pciused[1]) {
    PCICR |= PCIE1;
  }
  if (pciused[2]) {
    PCICR |= PCIE2;
  }
  sei();
}

/*
| Byte | Bit | Description |
| ----------- | ----------- |----------- |
| 0 | 0 | enable/disable this timer
| 0 | 1 | enable/diable output 0
| 0 | 2 | enable/diable output 1
| 0 | 3 | enable/diable output 2
| 0 | 4 |
| 0 | 5 |
| 0 | 6 |
| 0 | 7 |
| 1-2 | all | PWM frequency (0-65k)
| 3 | all | for output 0: assign the corresponding pinchange if RPM input is
used (255 for none) | 4 | all | for output 1: assign the corresponding
pinchange if RPM input is used (255 for none) | 5 | all | for output 2: assign
the corresponding pinchange if RPM input is used (255 for none)

*/
/*
16 bit timer calculation:
possible prescaler:
1: CS10
8: CS11
64: CS10 & CS11
256: CS12
1024: CS12 & CS10
algo:
1. check for prescaler to fit cycle into 16 bit counter
  cycles_per_s = F_CPU / 65536
  needed_prescaler = cycles_per_s / frequency
  if needed_precaler < 1024 -> prescaler = 1024
  if needed_precaler < 256 -> prescaler = 256
  if needed_precaler < 64 -> prescaler = 64
  if needed_precaler < 8 -> prescaler = 8
  if needed_precaler < 1 -> prescaler = 1
2. calc ICRx
  ( F_CPU / prescaler ) / frequency / 2
example 50Hz:
  needed_prescaler = ( 16 000 000 / 65535 ) / 50 = 4.88
  prescaler = 8
  ICR = ( 16 000 000 / 8 ) / 50 / 2 = 20 000
example 25 000Hz:
  needed_prescaler = ( 16 000 000 / 65535 ) / 25 000 = 0.00
  prescaler = 1
  ICR = ( 16 000 000 / 1 ) / 25 000 / 2 = 320
*/

void setupPWM0() {}

void setupPWM2() {}

void setupPWM1() {
  // using: pwmconfig[1];
  // 1: B5 / B6 / B7
  // timer1
  uint8_t config = pwmconfig[1][0];
  if (config & 1) {
    uint16_t frequency = pwmconfig[1][1] * 256 + pwmconfig[1][2];
    // check for already used pins and disable the usage here
    if (usedpins[1] & (1 << PORTB5)) {
      config &= ~(1 << PORTB5);
    }
    if (usedpins[1] & (1 << PORTB6)) {
      config &= ~(1 << PORTB6);
    }
    if (usedpins[1] & (1 << PORTB7)) {
      config &= ~(1 << PORTB7);
    }

    uint16_t needed_prescaler = (F_CPU / 2 ^ 16) / frequency;
    uint16_t prescaler;
    TCCR1B = _BV(WGM13); // PWM mode with ICR1 Mode 10
    if (needed_prescaler < 1) {
      TCCR1B |= _BV(CS10);
      prescaler = 1;
    } else if (needed_prescaler < 8) {
      TCCR1B |= _BV(CS11);
      prescaler = 8;
    } else if (needed_prescaler < 64) {
      TCCR1B |= _BV(CS11) | _BV(CS10);
      prescaler = 64;
    } else if (needed_prescaler < 256) {
      TCCR1B |= _BV(CS12);
      prescaler = 256;
    } else if (needed_prescaler < 1024) {
      TCCR1B |= _BV(CS12) | _BV(CS10);
      prescaler = 1024;
    }
    ICR1 = (F_CPU / prescaler) / frequency / 2;

    // set needed pin as output
    //    DDRB |= _BV(PORTB5) | _BV(PORTB6) | _BV(PORTB7);
    DDRB |= (((config >> 1) & 1) << PORTB5) | (((config >> 2) & 1) << PORTB6) |
            (((config >> 3) & 1) << PORTB7);
    TIMSK1 = 0;
    TIFR1 = 0;
    TCNT1 = 0;
    TCCR1A |=
        (((config >> 1) & 1) << COM1A1); // output A clear rising/set falling
    TCCR1A |=
        (((config >> 2) & 1) << COM1B1); // output B clear rising/set falling
    TCCR1A |=
        (((config >> 3) & 1) << COM1C1); // output C clear rising/set falling
    TCCR1A |= _BV(WGM11);                // WGM13:WGM10 set 1010
    usedpins[1] |= (((config >> 1) & 1) << PORTB5) |
                   (((config >> 2) & 1) << PORTB6) |
                   (((config >> 3) & 1) << PORTB7);
  } else {
    // disable timer
    TCCR1B = 0;
  }
}

void setupPWM3() {
  // using: pwmconfig[1];
  // 1: E3 / E4 / E5
  // timer1
  uint8_t config = pwmconfig[3][0];
  if (config & 1) {
    uint16_t frequency = pwmconfig[3][1] * 256 + pwmconfig[3][2];
    // check for already used pins and disable the usage here
    if (usedpins[3] & (1 << PORTE3)) {
      config &= ~(1 << PORTE3);
    }
    if (usedpins[3] & (1 << PORTE4)) {
      config &= ~(1 << PORTE4);
    }
    if (usedpins[3] & (1 << PORTE5)) {
      config &= ~(1 << PORTE5);
    }

    uint16_t needed_prescaler = (F_CPU / 2 ^ 16) / frequency;
    uint16_t prescaler;
    TCCR3B = _BV(WGM33); // PWM mode with ICR1 Mode 10
    if (needed_prescaler < 1) {
      TCCR3B |= _BV(CS30);
      prescaler = 1;
    } else if (needed_prescaler < 8) {
      TCCR3B |= _BV(CS31);
      prescaler = 8;
    } else if (needed_prescaler < 64) {
      TCCR3B |= _BV(CS31) | _BV(CS30);
      prescaler = 64;
    } else if (needed_prescaler < 256) {
      TCCR3B |= _BV(CS32);
      prescaler = 256;
    } else if (needed_prescaler < 1024) {
      TCCR3B |= _BV(CS32) | _BV(CS30);
      prescaler = 1024;
    }
    ICR3 = (F_CPU / prescaler) / frequency / 2;

    // set needed pin as output
    //    DDRB |= _BV(PORTE3) | _BV(PORTE4) | _BV(PORTE5);
    DDRE |= (((config >> 1) & 1) << PORTE3) | (((config >> 2) & 1) << PORTE4) |
            (((config >> 3) & 1) << PORTE5);
    TIMSK3 = 0;
    TIFR3 = 0;
    TCNT3 = 0;
    TCCR3A |=
        (((config >> 1) & 1) << COM3A1); // output A clear rising/set falling
    TCCR3A |=
        (((config >> 2) & 1) << COM3B1); // output B clear rising/set falling
    TCCR3A |=
        (((config >> 3) & 1) << COM3C1); // output C clear rising/set falling
    TCCR3A |= _BV(WGM31);                // WGM13:WGM10 set 1010
    usedpins[3] |= (((config >> 1) & 1) << PORTE3) |
                   (((config >> 2) & 1) << PORTE4) |
                   (((config >> 3) & 1) << PORTE5);
  } else {
    // disable timer
    TCCR3B = 0;
  }
}
void setupPWM4() {
  // using: pwmconfig[1];
  // 1: H3 / H4 / H5
  // timer1
  uint8_t config = pwmconfig[4][0];
  if (config & 1) {
    uint16_t frequency = pwmconfig[4][1] * 256 + pwmconfig[4][2];
    // check for already used pins and disable the usage here
    if (usedpins[4] & (1 << PORTH3)) {
      config &= ~(1 << PORTH3);
    }
    if (usedpins[4] & (1 << PORTH4)) {
      config &= ~(1 << PORTH4);
    }
    if (usedpins[4] & (1 << PORTH5)) {
      config &= ~(1 << PORTH5);
    }

    uint16_t needed_prescaler = (F_CPU / 2 ^ 16) / frequency;
    uint16_t prescaler;
    TCCR4B = _BV(WGM43); // PWM mode with ICR1 Mode 10
    if (needed_prescaler < 1) {
      TCCR4B |= _BV(CS40);
      prescaler = 1;
    } else if (needed_prescaler < 8) {
      TCCR4B |= _BV(CS41);
      prescaler = 8;
    } else if (needed_prescaler < 64) {
      TCCR4B |= _BV(CS41) | _BV(CS40);
      prescaler = 64;
    } else if (needed_prescaler < 256) {
      TCCR4B |= _BV(CS42);
      prescaler = 256;
    } else if (needed_prescaler < 1024) {
      TCCR4B |= _BV(CS42) | _BV(CS40);
      prescaler = 1024;
    }
    ICR4 = (F_CPU / prescaler) / frequency / 2;

    // set needed pin as output
    //    DDRB |= _BV(PORTH3) | _BV(PORTH4) | _BV(PORTEH5);
    DDRH |= (((config >> 1) & 1) << PORTH3) | (((config >> 2) & 1) << PORTH4) |
            (((config >> 3) & 1) << PORTH5);
    TIMSK4 = 0;
    TIFR4 = 0;
    TCNT4 = 0;
    TCCR4A |=
        (((config >> 1) & 1) << COM4A1); // output A clear rising/set falling
    TCCR4A |=
        (((config >> 2) & 1) << COM4B1); // output B clear rising/set falling
    TCCR4A |=
        (((config >> 3) & 1) << COM4C1); // output C clear rising/set falling
    TCCR4A |= _BV(WGM41);                // WGM13:WGM10 set 1010
    usedpins[3] |= (((config >> 1) & 1) << PORTE3) |
                   (((config >> 2) & 1) << PORTE4) |
                   (((config >> 3) & 1) << PORTE5);
  } else {
    // disable timer
    TCCR4B = 0;
  }
}

void setupPWM5() {
  // using: pwmconfig[1];
  // 1: L3 / L4 / L5
  // timer1
  uint8_t config = pwmconfig[5][0];
  if (config & 1) {
    uint16_t frequency = pwmconfig[5][1] * 256 + pwmconfig[5][2];
    // check for already used pins and disable the usage here
    if (usedpins[5] & (1 << PORTL3)) {
      config &= ~(1 << PORTL3);
    }
    if (usedpins[5] & (1 << PORTL4)) {
      config &= ~(1 << PORTL4);
    }
    if (usedpins[5] & (1 << PORTL5)) {
      config &= ~(1 << PORTL5);
    }

    uint16_t needed_prescaler = (F_CPU / 2 ^ 16) / frequency;
    uint16_t prescaler;
    TCCR5B = _BV(WGM53); // PWM mode with ICR1 Mode 10
    if (needed_prescaler < 1) {
      TCCR5B |= _BV(CS50);
      prescaler = 1;
    } else if (needed_prescaler < 8) {
      TCCR5B |= _BV(CS51);
      prescaler = 8;
    } else if (needed_prescaler < 64) {
      TCCR5B |= _BV(CS51) | _BV(CS50);
      prescaler = 64;
    } else if (needed_prescaler < 256) {
      TCCR5B |= _BV(CS52);
      prescaler = 256;
    } else if (needed_prescaler < 1024) {
      TCCR5B |= _BV(CS52) | _BV(CS50);
      prescaler = 1024;
    }
    ICR5 = (F_CPU / prescaler) / frequency / 2;

    // set needed pin as output
    //    DDRB |= _BV(PORTH3) | _BV(PORTH4) | _BV(PORTEH5);
    DDRL |= (((config >> 1) & 1) << PORTL3) | (((config >> 2) & 1) << PORTL4) |
            (((config >> 3) & 1) << PORTL5);
    TIMSK5 = 0;
    TIFR5 = 0;
    TCNT5 = 0;
    TCCR5A |=
        (((config >> 1) & 1) << COM5A1); // output A clear rising/set falling
    TCCR5A |=
        (((config >> 2) & 1) << COM5B1); // output B clear rising/set falling
    TCCR5A |=
        (((config >> 3) & 1) << COM5C1); // output C clear rising/set falling
    TCCR5A |= _BV(WGM51);                // WGM13:WGM10 set 1010
    usedpins[5] |= (((config >> 1) & 1) << PORTL3) |
                   (((config >> 2) & 1) << PORTL4) |
                   (((config >> 3) & 1) << PORTL5);
  } else {
    // disable timer
    TCCR5B = 0;
  }
}

// PCINT0 PORTB
ISR(PCINT0_vect) {
  /*
   pcintfallingmask
   pcintrisingmask
   */
  // XOR to check which pin changed and which are enabled for pinchange
  uint8_t flippedbyte = (pcintstate[0] ^ PINB) & PCMSK0;
  uint8_t addfalling = ~PINB & flippedbyte & pcintfallingmask[0];
  uint8_t addrising = PINB & flippedbyte & pcintrisingmask[0];

  pccounter[0] += (addfalling & 0x01) >> 0 + (addrising & 0x01) >> 0;
  pccounter[1] += (addfalling & 0x02) >> 1 + (addrising & 0x02) >> 1;
  pccounter[2] += (addfalling & 0x04) >> 2 + (addrising & 0x04) >> 2;
  pccounter[3] += (addfalling & 0x08) >> 3 + (addrising & 0x08) >> 3;
  pccounter[4] += (addfalling & 0x10) >> 4 + (addrising & 0x10) >> 4;
  pccounter[5] += (addfalling & 0x20) >> 5 + (addrising & 0x20) >> 5;
  pccounter[6] += (addfalling & 0x40) >> 6 + (addrising & 0x40) >> 6;
  pccounter[7] += (addfalling & 0x80) >> 7 + (addrising & 0x80) >> 7;
  pcintstate[0] = PINB;
}

// PCINT1 PORTE0 ; PORTJ0-PORTJ6
ISR(PCINT1_vect) {
  /*
   pcintfallingmask
   pcintrisingmask
   */
  // XOR to check which pin changed and which are enabled for pinchange
  uint8_t input = (PORTJ << 1) | (PORTE & 1);
  uint8_t flippedbyte = (pcintstate[1] ^ input) & PCMSK0;
  uint8_t addfalling = ~input & flippedbyte & pcintfallingmask[0];
  uint8_t addrising = input & flippedbyte & pcintrisingmask[0];

  pccounter[8] += (addfalling & 0x01) >> 0 + (addrising & 0x01) >> 0;
  pccounter[9] += (addfalling & 0x02) >> 1 + (addrising & 0x02) >> 1;
  pccounter[10] += (addfalling & 0x04) >> 2 + (addrising & 0x04) >> 2;
  pccounter[11] += (addfalling & 0x08) >> 3 + (addrising & 0x08) >> 3;
  pccounter[12] += (addfalling & 0x10) >> 4 + (addrising & 0x10) >> 4;
  pccounter[13] += (addfalling & 0x20) >> 5 + (addrising & 0x20) >> 5;
  pccounter[14] += (addfalling & 0x40) >> 6 + (addrising & 0x40) >> 6;
  pccounter[15] += (addfalling & 0x80) >> 7 + (addrising & 0x80) >> 7;
  pcintstate[1] = input;
}

// PCINT2 PORTK
ISR(PCINT2_vect) {
  /*
   pcintfallingmask
   pcintrisingmask
   */
  // XOR to check which pin changed and which are enabled for pinchange
  uint8_t flippedbyte = (pcintstate[2] ^ PINK) & PCMSK2;
  uint8_t addfalling = ~PINK & flippedbyte & pcintfallingmask[0];
  uint8_t addrising = PINK & flippedbyte & pcintrisingmask[0];

  pccounter[16] += (addfalling & 0x01) >> 0 + (addrising & 0x01) >> 0;
  pccounter[17] += (addfalling & 0x02) >> 1 + (addrising & 0x02) >> 1;
  pccounter[18] += (addfalling & 0x04) >> 2 + (addrising & 0x04) >> 2;
  pccounter[19] += (addfalling & 0x08) >> 3 + (addrising & 0x08) >> 3;
  pccounter[20] += (addfalling & 0x10) >> 4 + (addrising & 0x10) >> 4;
  pccounter[21] += (addfalling & 0x20) >> 5 + (addrising & 0x20) >> 5;
  pccounter[22] += (addfalling & 0x40) >> 6 + (addrising & 0x40) >> 6;
  pccounter[23] += (addfalling & 0x80) >> 7 + (addrising & 0x80) >> 7;
  pcintstate[2] = PINK;
}

ISR(ADC_vect) {
  currentanalogvalue[currentanalogcounter] = ADCL + ADCH * 256;

  // do {
  //   currentanalogcounter = (currentanalogcounter + 1) % 16;
  // } while (! adconfig[currentanalogcounter / 8][currentanalogcounter % 8])
  // TODO trigger next reading
}
