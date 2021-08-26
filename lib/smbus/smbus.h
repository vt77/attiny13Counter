/**
 *
 *  Software emulated USI TWI smbus communication   
 *  All the code runs inside interrupt subroutine, so it's transparent for another processes
 *  and can't be interrupted during execution  
 * 
 */

#ifndef  __VT77_TINY_SMBUS
#define  __VT77_TINY_SMBUS

#include <avr/io.h>
#include  <avr/interrupt.h>

#ifndef SDA
#define SDA PB1
#endif

#ifndef SCL
#define SCL PB0
#endif

#ifndef SLAVE_ADDRESS
#define SLAVE_ADDRESS 0x15
#endif


#ifndef DEVICE_DATA_LENGTH
#define DEVICE_DATA_LENGTH 8
#endif

void smbus_slave_init( void );
void smbus_slave_enable( void );
void smbus_slave_disable( void );

#endif
