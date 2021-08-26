Low power smbus (i2c) slave counter for attiny13
--- 

## Introduction

This project is a very simple 16 bit counter with i2c interface designed for low power consumation. Most time counter sleeps and counting ticks in async timer mode. It interfaces SMBus protocol to read out counter value

## Usage 
I used it in my WeatherStation project to count anemometer ticks. This project not intended to run in production environment because it missing some important functions, but it's still good for DIY projects, tested and runs pretty good in my case, capable low consumption and good stability.

## Compile 
avr-gcc should be installed to compile this project. 

```
make && make fuses && make upload
```

## Hardware 

CNT - 8bit Timer0 counter pin

SDA - External interrupt pin (wakeups microcontroller for communication)

SCL - Any GPIO pin ( PB0 by default )

```
       Attiny13a 
        -------          
      -|RST VCC|-         
      -|PB3 PB2|- CNT        
      -|PB4 PB1|- SDA    
      -|GND PB0|- SCL      
        ------- 
```
## License
MIT. See LICENSE file
