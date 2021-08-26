SMBUS I2C implementation for attiny13 
--- 

## Introduction

This library very basic implementation of smbus protocol for attiny13 microcontroller
It should work pretty good on low speed 100kHz bus, no stretching needed. 

It disigned for low power consumation and may work in sleep mode. INT0 interrupt used to detect start condition and wake-up microcontroller. 

*PEC not supported*

## SMBUS Functions:
**PLEASE NOTE!**  There is some differences from original smbus protocol<br/>
The device has 2 memory slots for read and write. Smbus command, actually is a mem ptr to read/write operarion.Internal counter always incremented on ACK, so *data* <u>may have multiply bytes</u> in all functions until NACK reached.It means you can read as many bytes as needed for your process<br/>
Counter always resetted to 0 on Stop condition. 


Following smbus functions implemented in firmware.


### smbus_read_byte 
```
S Addr Rd [A] [Data] NA P
```
Sends *Data* to host controller.
### smbus_write_byte
```
S Addr Wr [A] Data [A] P
```
Reads *Data* from host controller. 

### smbus_read_byte_data/smbus_read_word_data 
```
S Addr Wr [A] Comm [A] S Addr Rd [A] [Data] NA P
```
Sends *Data* to host controller from specified address. 

### smbus_read_byte_data/smbus_read_word_data 
```
S Addr Wr [A] Comm [A] S Addr Rd [A] [Data] NA P
```
Reads *Data* from host controller into specified address. 


## Hardware 

SDA - External interrupt pin (wakeups microcontroller for communication)

SCL - Any GPIO pin ( PB0 by default )

```
       Attiny13a 
        -------          
      -|RST VCC|-         
      -|PB3 PB2|-         
      -|PB4 PB1|- SDA    
      -|GND PB0|- SCL      
        ------- 
```
## License
MIT. See LICENSE file
