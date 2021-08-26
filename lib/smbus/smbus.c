

#include "smbus.h"

#define clear_bit(a,b) a &= ~_BV(b)
#define set_bit(a,b) a |= _BV(b)


// TWI macros
#define TWI_SDA_HIGH()  DDRB &= ~_BV(SDA) // release SDA   -> pulled HIGH by resistor
#define TWI_SDA_LOW()   DDRB |=  _BV(SDA) // SDA as output -> pulled LOW  by MCU
#define TWI_SCL_HIGH()  DDRB &= ~_BV(SCL) // release SCL   -> pulled HIGH by resistor
#define TWI_SCL_LOW()   DDRB |=  _BV(SCL) // SCL as output -> pulled LOW  by MCU


/* External interrupt macros. These are device dependent. */
#define INITIALIZE_TWI_INTERRUPT()    (MCUCR |= _BV(ISC01))  //!< Sets falling edge of SDA generates interrupt.
//#define INITIALIZE_TWI_INTERRUPT()    (MCUCR = 0) //Low level may wake-up MCU from POWER_DOWN
#define ENABLE_TWI_INTERRUPT()        (GIMSK |= _BV(INT0))   //!< Enables SDA interrupt.
#define DISABLE_TWI_INTERRUPT()       (GIMSK &= ~_BV(INT0))  //!< Disables SDA interrupt.
#define CLEAR_TWI_INTERRUPT_FLAG()    (GIFR = _BV(INTF0))    //!< Clears interrupt flag.


#define READ_SDA()              (PINB & (1 << SDA))      //!< read pin value
#define READ_SCL()              (PINB & (1 << SCL))

#define SDA_vector  INT0_vect  //!< Interrupt vector for SDA pin

//state register 
register uint8_t state asm("r2");
//Memory ptr
register uint8_t data_ptr asm("r3");


#define STATE_IDLE 0
#define STATE_CHECK_ADDRESS 1
#define STATE_READ   ((SLAVE_ADDRESS << 1) | 1) 
#define STATE_WRITE  ((SLAVE_ADDRESS << 1) | 0)

uint8_t readBuffer[DEVICE_DATA_LENGTH] = {0x1,0x2,0x3};
uint8_t writeBuffer[DEVICE_DATA_LENGTH] = {0x1,0x2,0x3};


//This function should be defined in main function
void prepare_write_data(uint8_t * data);


/**
 *  Init TWI Interrupt   
 */
void smbus_slave_init( void )
{

    //All input 
    clear_bit(DDRB,SDA);
    clear_bit(DDRB,SCL);
    //Hi-z state
    clear_bit(PORTB,SDA);
    clear_bit(PORTB,SCL);
    
    TWI_SDA_HIGH();
    TWI_SCL_HIGH();

    INITIALIZE_TWI_INTERRUPT();    
    state = STATE_IDLE;
}

void smbus_slave_enable( void )
{
  CLEAR_TWI_INTERRUPT_FLAG();
  ENABLE_TWI_INTERRUPT();
}

void smbus_slave_disable( void )
{
  DISABLE_TWI_INTERRUPT();
  CLEAR_TWI_INTERRUPT_FLAG();
}


void send_ack()
{
    TWI_SDA_LOW();
    while(!READ_SCL());
    while(READ_SCL());
    TWI_SDA_HIGH();
}

void check_ack()
{
   //Reset data_ptr on NACK
   while(!READ_SCL());
   if(READ_SDA())
   {
      state =  STATE_IDLE;
      data_ptr = 0;
   }
   while(READ_SCL());
}

uint8_t twi_process(uint8_t byte)
{
    uint8_t i,bit;
    for(i=0;i<8;i++)
    {  
      if( !(byte & 0x80 ) )
          TWI_SDA_LOW();
      else
	      TWI_SDA_HIGH();

      while(!READ_SCL());

      bit  = READ_SDA(); 
      while(READ_SCL()){
          if(READ_SDA() != bit)
          {
              //Stop or Sr detected
              state = bit ? STATE_CHECK_ADDRESS : STATE_IDLE;
              return 0;
          }
      }

      byte <<= 1;	
      if(bit)
          byte |= 1;
    }

    TWI_SDA_HIGH();    
    return  byte;
}


/**
 *  START - SDA transition from  HIGH to LOW when SCL is High
 *  STOP  - SDA transition from LOW to HIGH when SCL is High
 */

void  twi_read_address_rw()
{
   while(READ_SCL()); 
   state = twi_process(0xFF); 
   //Wrong address. Skip
   if(( state & 0xFE) != (SLAVE_ADDRESS << 1))
      return;
   
  //TODO: Stretch SCL and prepare data
   prepare_write_data(writeBuffer);
   send_ack();

   if(state == STATE_WRITE)
   {
      //Read register address to read/write
      data_ptr = twi_process(0xFF);
      if(data_ptr >= DEVICE_DATA_LENGTH || state != STATE_WRITE)
	       return;
      send_ack();
   }   
}

/**   
 *  This interrupt wakes-up microcontroller and starts I2C transaction process
 */
ISR(SDA_vector) {

    uint8_t b;

   //Check start sequence
   if(!READ_SCL())
       goto finish_isr;

  state =  STATE_CHECK_ADDRESS;
  while(READ_SCL());  

  while(STATE_IDLE != state){
    switch (state)
    {
        case STATE_CHECK_ADDRESS:
            twi_read_address_rw();
            break;

        case STATE_WRITE:
             b = twi_process(0xFF);
             if(state == STATE_WRITE )
             {
                readBuffer[data_ptr++] = b;
                if(data_ptr < DEVICE_DATA_LENGTH )
                send_ack();
             }
		    break;
		
        case STATE_READ:
            twi_process(writeBuffer[data_ptr++]);
	        check_ack();
		    break;
        default:
            state =  STATE_IDLE;
	    break;
    }
  }

finish_isr:
  CLEAR_TWI_INTERRUPT_FLAG();
  ENABLE_TWI_INTERRUPT();
}

