/*
 * RC_CAR.c
 *
 * Created: 5/5/2020 10:46:24 PM
 * Author : Ramy Shaaban
 */ 
#define F_CPU 1000000
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "USART.h"
#include "DCMTR.h"

#define PULSE_MIN         800         /* experiment with these values */ 
#define PULSE_MAX         2200              /* to match your own servo */ 
#define PULSE_MID         1500


// Pin mapping
/*
                       L293D
			          --- ---
		N/a        <- |     | -> VCC ~ 3.7*2 ~ 8.5V
		PD3        <- |     | -> PD5
		R motor 1  <- |     | -> l motor 1
		GND        <- |     | -> GND
		GND        <- |     | -> GND
		R motor 2  <- |     | -> l motor 2
		PB3        <- |     | -> PD6
		VCC ~ 8.5V <- |     | -> N/A
			          -------
			  
*/
/*            Bluetooth
              ---------
    RX (Bluetooth) -> TX PD1
	TX (Bluetooth) -> RX PD0
	VCC   ~ 3.7V ~ 4.25V  one Li battery
	GND   -> GND
*/

/*           Ultrasonic
             ---------- 
	TRIG  -> PD2 (INT0)
	ECHO  -> PC5 
	VCC   ~ 3.7V ~ 4.25V  one Li battery
	GND   -> GND 
*/

/*           Atmega328P
   The main controller all other components connected to it
   VCC   ~ 3.5V ~ 4.25V  one Li battery
   GND   -> GND 
*/

static volatile long num_of_pulses = 0;   //mandatory to be long to give correct sensor distance reading
static volatile int update = 0;
  


void Ultrasonic_init(){
	 DDRD &= ~(1 << PIND2);    // Used as Echo pin of ultrasonic
	 DDRC |= (1 << PINC5);     // Trig pin of ultrasonic
	  _delay_ms(50);           // delay

     EIMSK |= (1 << INT0);     // External control interrupt register.
	 EICRA |= (1 << ISC00);
	 
}

//Take reading from Ultrasonic sensor
void Take_Reading(){
	PORTC |= (1 << PINC5); 
	_delay_us(15);
	PORTC &= ~(1 << PINC5);
}
 ISR(INT0_vect)
 {
	 
	if(update == 1)
	 {
		 TCCR1B &= ~(1 << CS10);    //Disabling Timer/Counter  
		 num_of_pulses = TCNT1; //Read the value from the sensor
		 TCNT1 = 0;     //Reset
		 update = 0;  
	 }

	 if(update == 0)
	 {
		 TCCR1B |= 1<<CS10;  //Start counting in microseconds
		 update = 1;
	 }
	 
	 
 }
 
  // Using Timer 1 To control servo motor using Fast PWM
 static inline void initTimer1Servo(void) {
	 
	 /* Set up Timer1 (16bit) to give a pulse every 20ms */
	 /* Use Fast PWM mode, counter max in ICR1 */
	 TCCR1A |= (1 << WGM11);
	 TCCR1B |= (1 << WGM12) | (1 << WGM13);
	 TCCR1B |= (1 << CS10);  /* /1 prescaling -- counting in microseconds */
	 ICR1 = 20000;                                    /* TOP value = 20ms */
	 TCCR1A |= (1 << COM1A1);
	 /* Direct output on PB1 / OC1A */
	 DDRB |= (1 << PINB1);                            /* set pin for output */
 }
 
 
int main(void)
{
	// intialization steps
	LD293_init();
	USART_init(9600 ,F_CPU);   // X parameters aren't used for more info, Go to USART.c
	L293D_setSpeed(255);       // Set initial speed as maximum
    Ultrasonic_init();         // initialize ultrasonic to be used in Timer interrupt mode
	initTimer1Servo();         // initialize Servo motor
	sei();
	char show_a[16];  //debugging character array
	/* int obstacle_last = 0; */  // X not used variable
 
    while (1) 
    {  
		// take readings
		int i ;
		distance = 0;
		for(i = 0; i < 10 ; i++){
		Take_Reading();
		distance +=  num_of_pulses / 58;
		_delay_ms(10);
		}
		
		distance = distance / 10;
		// For debugging purposes I printed the distance 
		itoa(distance,show_a,10);
		USART_sendString(show_a);
		USART_send('\n');
		
		 OCR1A = PULSE_MID;  // make the motor at the middle
		 //_delay_ms(1000);
		// Mandatory delay for the reading of the sensor 
	   
		
		 /* cli();
		  initTimer1Servo();*/
		  //OCR1A = PULSE_MIN;
		 // _delay_ms(1500);
		 // sei();
		  
		  /* cli();
		   initTimer1Servo();*/
		  // OCR1A = PULSE_MAX;
		  // _delay_ms(1500);
		  // sei();*/
		 
		 
		/* Stop the RC car if there is an obstacle
		 * when the distance is less than 25
		 * Stop the car
		 */
		
		// If RC Car is not at STOP state
		if(Stop_flag == 0){
		if(distance < 30){  // if distance is less than threshold
		 L293D_stop();                    // Stop the` car

		  initTimer1Servo();              
		 
		  
		   OCR1A = PULSE_MIN;             // Move the servo motor right (ultrasonic)
		  _delay_ms(1000);                // wait                   
		 
		  
		  
		  distance = 0;
		  for(i = 0; i < 10 ; i++){
			  Take_Reading();
			  distance +=  num_of_pulses / 58;
			  _delay_ms(10);
		  }
		  
		  distance = distance / 10;
		  
		  
		  itoa(distance,show_a,10);
		  USART_sendString(show_a);
		  USART_send('\n');
		  
		  
		  if(distance > 50){              // If distance at right is greater than 60
		   L293D_setSpeed(255);           // Set speed to max
		   L293D_moveRight();             // Move right                 
		   _delay_ms(500);                  
		   L293D_setSpeed(150);           // Set speed to a reasonable value
		  L293D_stop();
		  Stop_flag = 1;                  // the car is at stop state waiting for external command to move
		  
		  }
		  else{
             
			  
			  OCR1A = PULSE_MAX;
			 _delay_ms(1000);
			
			  distance = 0;
			  for(i = 0; i < 10 ; i++){
				  Take_Reading();
				  distance +=  num_of_pulses / 58;
				  _delay_ms(10);
			  }
			  
			  distance = distance / 10;
			 
			  
			  itoa(distance,show_a,10);
			  USART_sendString(show_a);
			  USART_send('\n');
			  
			  if(distance > 50){
				L293D_setSpeed(255);
			    L293D_moveLeft();
				_delay_ms(500);
				L293D_setSpeed(150);
				L293D_stop();
				Stop_flag = 1;
			  }
		  }
		 
		}
		 
	  }
	}
}

