# esphomesuperarduino

## Info

This code enables you to control a wide range of useful functions for controlling stuff like in a smart home environment.

The initial purpose was to control Servos for ventilation shutters(50Hz pwm), control ventilation fans and read there speed (25kHz pwm and pulsecounter) and have some simple I/O to check door sensors.

## Control

If a supported atmega-chip (or Arduino) is flashed with this firmware it will be only controlled via I2C interface.
The default I2C address is 0x0c and can be changed do to pin assignments or by changing it via I2C.

## Registeroverview

| Register | Read/Write | Length (bytes) | Description |
| ----------- | ----------- | ----------- |----------- |
| 0x00 | r | 1 | version of this code
| 0x01 | r | 1 | type of microcontroller
| 0x02 | r/w | 1 | i2c address
| 0x10 | r/w | 6 | Configuration of the PWM generator 0 <br>(not yet implemented)
| 0x11 | r/w | 6 | Configuration of the PWM generator 1 <br>(16 bit timer) <br> see details below
| 0x12 | r/w | 6 | Configuration of the PWM generator 2 <br>(not yet implemented)
| 0x13 | r/w | 6 | Configuration of the PWM generator 3 <br>(16 bit timer) <br> see details below
| 0x14 | r/w | 6 | Configuration of the PWM generator 4 <br>(16 bit timer) <br> see details below
| 0x15 | r/w | 6 | Configuration of the PWM generator 5 <br>(16 bit timer) <br> see details below
| 0x30 | r/w | 1 | PWM generator 0 output 0 : set/get the duty (0-255)
| 0x31 | r/w | 1 | PWM generator 0 output 1 : set/get the duty (0-255)
| 0x32 | r/w | 1 | PWM generator 0 output 2 : set/get the duty (0-255)
| 0x33 | r/w | 1 | PWM generator 1 output 0 : set/get the duty (0-255)
| 0x34 | r/w | 1 | PWM generator 1 output 1 : set/get the duty (0-255)
| 0x35 | r/w | 1 | PWM generator 1 output 2 : set/get the duty (0-255)
| 0x36 | r/w | 1 | PWM generator 2 output 0 : set/get the duty (0-255)
| 0x37 | r/w | 1 | PWM generator 2 output 1 : set/get the duty (0-255)
| 0x38 | r/w | 1 | PWM generator 2 output 2 : set/get the duty (0-255)
| 0x39 | r/w | 1 | PWM generator 3 output 0 : set/get the duty (0-255)
| 0x3a | r/w | 1 | PWM generator 3 output 1 : set/get the duty (0-255)
| 0x3b | r/w | 1 | PWM generator 3 output 2 : set/get the duty (0-255)
| 0x3c | r/w | 1 | PWM generator 4 output 0 : set/get the duty (0-255)
| 0x3d | r/w | 1 | PWM generator 4 output 1 : set/get the duty (0-255)
| 0x3e | r/w | 1 | PWM generator 4 output 2 : set/get the duty (0-255)
| 0x3f | r/w | 1 | PWM generator 5 output 0 : set/get the duty (0-255)
| 0x40 | r/w | 1 | PWM generator 5 output 1 : set/get the duty (0-255)
| 0x41 | r/w | 1 | PWM generator 5 output 2 : set/get the duty (0-255)
| 0x50 | r/w | 2 | PWM generator 0 output 0 : set/get the duty (0-65535)
| 0x51 | r/w | 2 | PWM generator 0 output 1 : set/get the duty (0-65535)
| 0x52 | r/w | 2 | PWM generator 0 output 2 : set/get the duty (0-65535)
| 0x53 | r/w | 2 | PWM generator 1 output 0 : set/get the duty (0-65535)
| 0x54 | r/w | 2 | PWM generator 1 output 1 : set/get the duty (0-65535)
| 0x55 | r/w | 2 | PWM generator 1 output 2 : set/get the duty (0-65535)
| 0x56 | r/w | 2 | PWM generator 2 output 0 : set/get the duty (0-65535)
| 0x57 | r/w | 2 | PWM generator 2 output 1 : set/get the duty (0-65535)
| 0x58 | r/w | 2 | PWM generator 2 output 2 : set/get the duty (0-65535)
| 0x59 | r/w | 2 | PWM generator 3 output 0 : set/get the duty (0-65535)
| 0x5a | r/w | 2 | PWM generator 3 output 1 : set/get the duty (0-65535)
| 0x5b | r/w | 2 | PWM generator 3 output 2 : set/get the duty (0-65535)
| 0x5c | r/w | 2 | PWM generator 4 output 0 : set/get the duty (0-65535)
| 0x5d | r/w | 2 | PWM generator 4 output 1 : set/get the duty (0-65535)
| 0x5e | r/w | 2 | PWM generator 4 output 2 : set/get the duty (0-65535)
| 0x5f | r/w | 2 | PWM generator 5 output 0 : set/get the duty (0-65535)
| 0x60 | r/w | 2 | PWM generator 5 output 1 : set/get the duty (0-65535)
| 0x61 | r/w | 2 | PWM generator 5 output 2 : set/get the duty (0-65535)
| 0x70 | r/w | 2 | PWM generator 0 output 0 : set/get the wanted rpm (0-65535)
| 0x71 | r/w | 2 | PWM generator 0 output 1 : set/get the wanted rpm (0-65535)
| 0x72 | r/w | 2 | PWM generator 0 output 2 : set/get the wanted rpm (0-65535)
| 0x73 | r/w | 2 | PWM generator 1 output 0 : set/get the wanted rpm (0-65535)
| 0x74 | r/w | 2 | PWM generator 1 output 1 : set/get the wanted rpm (0-65535)
| 0x75 | r/w | 2 | PWM generator 1 output 2 : set/get the wanted rpm (0-65535)
| 0x76 | r/w | 2 | PWM generator 2 output 0 : set/get the wanted rpm (0-65535)
| 0x77 | r/w | 2 | PWM generator 2 output 1 : set/get the wanted rpm (0-65535)
| 0x78 | r/w | 2 | PWM generator 2 output 2 : set/get the wanted rpm (0-65535)
| 0x79 | r/w | 2 | PWM generator 3 output 0 : set/get the wanted rpm (0-65535)
| 0x7a | r/w | 2 | PWM generator 3 output 1 : set/get the wanted rpm (0-65535)
| 0x7b | r/w | 2 | PWM generator 3 output 2 : set/get the wanted rpm (0-65535)
| 0x7c | r/w | 2 | PWM generator 4 output 0 : set/get the wanted rpm (0-65535)
| 0x7d | r/w | 2 | PWM generator 4 output 1 : set/get the wanted rpm (0-65535)
| 0x7e | r/w | 2 | PWM generator 4 output 2 : set/get the wanted rpm (0-65535)
| 0x7f | r/w | 2 | PWM generator 5 output 0 : set/get the wanted rpm (0-65535)
| 0x80 | r/w | 2 | PWM generator 5 output 1 : set/get the wanted rpm (0-65535)
| 0x81 | r/w | 2 | PWM generator 5 output 2 : set/get the wanted rpm (0-65535)
| 0x90 | r/w | 1 | pinchange 0 : set/get config <br> see details below
| 0x91 | r/w | 1 | pinchange 1 : set/get config <br> see details below
| 0x92 | r/w | 1 | pinchange 2 : set/get config <br> see details below
| 0x93 | r/w | 1 | pinchange 3 : set/get config <br> see details below
| 0x94 | r/w | 1 | pinchange 4 : set/get config <br> see details below
| 0x95 | r/w | 1 | pinchange 5 : set/get config <br> see details below
| 0x96 | r/w | 1 | pinchange 6 : set/get config <br> see details below
| 0x97 | r/w | 1 | pinchange 7 : set/get config <br> see details below
| 0x98 | r/w | 1 | pinchange 8 : set/get config <br> see details below
| 0x99 | r/w | 1 | pinchange 9 : set/get config <br> see details below
| 0x9a | r/w | 1 | pinchange 10 : set/get config <br> see details below
| 0x9b | r/w | 1 | pinchange 11 : set/get config <br> see details below
| 0x9c | r/w | 1 | pinchange 12 : set/get config <br> see details below
| 0x9d | r/w | 1 | pinchange 13 : set/get config <br> see details below
| 0x9e | r/w | 1 | pinchange 14 : set/get config <br> see details below
| 0x9f | r/w | 1 | pinchange 15 : set/get config <br> see details below
| 0xa0 | r/w | 1 | pinchange 16 : set/get config <br> see details below
| 0xa1 | r/w | 1 | pinchange 17 : set/get config <br> see details below
| 0xa2 | r/w | 1 | pinchange 18 : set/get config <br> see details below
| 0xa3 | r/w | 1 | pinchange 19 : set/get config <br> see details below
| 0xa4 | r/w | 1 | pinchange 20 : set/get config <br> see details below
| 0xa5 | r/w | 1 | pinchange 21 : set/get config <br> see details below
| 0xa6 | r/w | 1 | pinchange 22 : set/get config <br> see details below
| 0xa7 | r/w | 1 | pinchange 23 : set/get config <br> see details below
| 0xb0 | r/w | 1 | analog digital converter 0-7 <br> enable/disable the corresponding bit 
| 0xb1 | r/w | 1 | analog digital converter 8-15 <br> enable/disable the corresponding bit 
| 0xc0 | r/w | 2 | digital I/O port A <br> see details below
| 0xc1 | r/w | 2 | digital I/O port B <br> see details below
| 0xc2 | r/w | 2 | digital I/O port C <br> see details below
| 0xc3 | r/w | 2 | digital I/O port D <br> see details below
| 0xc4 | r/w | 2 | digital I/O port E <br> see details below
| 0xc5 | r/w | 2 | digital I/O port F <br> see details below
| 0xc6 | r/w | 2 | digital I/O port G <br> see details below
| 0xc7 | r/w | 2 | digital I/O port H <br> see details below
| 0xc8 | r/w | 2 | digital I/O port I <br> see details below    (not populated)
| 0xc9 | r/w | 2 | digital I/O port J <br> see details below
| 0xca | r/w | 2 | digital I/O port K <br> see details below
| 0xcb | r/w | 2 | digital I/O port L <br> see details below



### PWM generator configuration (0x10 - 0x15)

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
| 3 | all | for output 0: assign the corresponding pinchange if RPM input is used (255 for none)
| 4 | all | for output 1: assign the corresponding pinchange if RPM input is used (255 for none)
| 5 | all | for output 2: assign the corresponding pinchange if RPM input is used (255 for none)


### pinchange configuration (0x90 - 0xa7)

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

### I/O configuration (0xc0 - 0xcb)

| Byte | Bit | Description |
| ----------- | ----------- |----------- |
| 0 | 0 | PIN 0: 0 input ; 1 output
| 0 | 1 | PIN 1: 0 input ; 1 output
| 0 | 2 | PIN 2: 0 input ; 1 output
| 0 | 3 | PIN 3: 0 input ; 1 output
| 0 | 4 | PIN 4: 0 input ; 1 output
| 0 | 5 | PIN 5: 0 input ; 1 output
| 0 | 6 | PIN 6: 0 input ; 1 output
| 0 | 7 | PIN 7: 0 input ; 1 output
| 1 | 0 | PIN 0: pullup enabled
| 1 | 1 | PIN 1: pullup enabled
| 1 | 2 | PIN 2: pullup enabled
| 1 | 3 | PIN 3: pullup enabled
| 1 | 4 | PIN 4: pullup enabled
| 1 | 5 | PIN 5: pullup enabled
| 1 | 6 | PIN 6: pullup enabled
| 1 | 7 | PIN 7: pullup enabled

