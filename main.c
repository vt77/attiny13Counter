
#include <stdint.h>
#include <avr/sleep.h>

#include <smbus/smbus.h>
#include <counter/counter.h>


//extern uint8_t writeBuffer[DEVICE_DATA_LENGTH];
//extern uint8_t readBuffer[DEVICE_DATA_LENGTH];


void prepare_write_data(uint8_t * data)
{
    data[0] = counter;
    data[1] = TCNT0;
}


int main()
{

    ADCSRA &= ~(1<<ADEN); //ADC OFF
    ACSR |= _BV(ACD);     //Comparator OFF
    PRR=(0<<PRTIM0)|(1<<PRADC); //Disable ADC
    //BODCR |= _BV(BODS); Disable BOD

    //DIDR0 |= _BV(AIN1D) | _BV(AIN0D); //Disable digital inputs 
    DDRB = 0x00; //Default all pins input with pullups for power save
    PORTB = 0xFF;

    smbus_slave_init();
    smbus_slave_enable();
    counter_init();

    set_sleep_mode(SLEEP_MODE_IDLE /*SLEEP_MODE_PWR_DOWN*/);
#ifdef sleep_bod_disable     
    sleep_bod_disable();
#endif

    sei();

    while(1)
    {
        sleep_enable();
        sleep_cpu();
        sleep_disable();
    }
}

