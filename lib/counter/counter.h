
#include <avr/io.h>
#include  <avr/interrupt.h>
#include <stdint.h>


//register uint8_t counter asm("r6");

volatile uint8_t counter = 0;

void counter_init()
{
    // set prescale timer to 1/1024th 
   TCCR0B |= (1<<CS02) | (1<<CS00);// set CS02 and CS00 bit in TCCR0B
   // enable timer overflow interrupt
   TIMSK0 |=1<<TOIE0;
}

ISR(TIM0_OVF_vect) {
    counter++;
}
