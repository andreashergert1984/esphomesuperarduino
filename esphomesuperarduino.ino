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
uint8_t usedpins[12] = {0};    // an array to store all used pins on setupAll to
                               // prevent configuring the same pin twice

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
  Wire.begin(i2caddress); // join i2c bus with address #4
  //  Wire.onReceive(receiveEvent); // register event
  //  Wire.onRequest(requestEvent); // register event

  // setup PWM Output
  setupPWM0();
  setupPWM1();
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

void setupPWM0() {}

void setupPWM1() {
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
used (255 for none) | 4 | all | for output 1: assign the corresponding pinchange
if RPM input is used (255 for none) | 5 | all | for output 2: assign the
corresponding pinchange if RPM input is used (255 for none)

  */
  // using: pwmconfig[1];
  // 1: B5 / B6 / B7
  // timer1
  uint8_t config = pwmconfig[1][0];
  if (config & 1) {
    uint16_t frequency = pwmconfig[1][1] * 256 + pwmconfig[1][2];
    // check for already used pins and disable the usage here
    if (usedpins[1] & (1 << PORTB5)) {
      config &= (0 << PORTB5);
    }
    if (usedpins[1] & (1 << PORTB6)) {
      config &= (0 << PORTB6);
    }
    if (usedpins[1] & (1 << PORTB7)) {
      config &= (0 << PORTB7);
    }
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
