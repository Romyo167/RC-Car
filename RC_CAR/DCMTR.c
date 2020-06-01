#include "DCMTR.h"
#include <avr/io.h>

/*This driver works as PWM controller for the motor, so we have specifically
4 pins that works with PWM in atmega32A, Those pins are OC0,OC2,OC1A and OC2A*/
/*
  OC0  -> PB3  -> 1A   ------------------- PD5
  OC2  -> PD7  -> 2A   ------------------- PD6
  OC1A -> PD5  -> 1B   ------------------- PB3
  OC1B -> PD4  -> 2B   ------------------- PD3
  
  
  Later I used atmega328p
  
  OC0A  -> PD5  -> 1A
  OC0B  -> PD6  -> 2A
  OC2A -> PB3   -> 1B
  OC2B -> PD3   -> 2B
*/
int spd;
int speed_to_distance ;
void LD293_init( ){
	/*DDRB |= (1 << PINB3);
	DDRD |= (1 << PIND7) | (1 << PIND5) | (1 << PIND4);*/
	
	TCCR0A |= (1 << WGM01) | (1 << WGM00); // timer0 fast pwm
	TCCR0A |= (1 << COM0A1);                // timer0 non-inverted mode
	TCCR0A |= (1 << COM0B1);
	TCCR0B |= (1 << CS01);                 //  /8 prescalar
	
	//same for timer 2
	TCCR2A |= (1 << WGM21) | (1 << WGM20);
	TCCR2A |= (1 << COM2A1);
	TCCR2A |= (1 << COM2B1);
	TCCR2B |= (1 << CS21);
	
	 
}

// speed value between 0 ~ 255
void L293D_setSpeed(int speed){
	spd = speed;
	
	speed_to_distance = 30 * speed / 255 ;  // set speed to distance
}

void L293D_moveForward(){
	OCR0A = spd;
	OCR0B = spd;
	OCR2A = spd;
	OCR2B = spd;
	DDRD |= ( 1 << PIND5);
	DDRB |= (1 << PINB3);
	DDRD &=  ~((1 << PIND6) | (1 << PIND3));
	
}

void L293D_moveBackward(){
	OCR0A = spd;
	OCR0B = spd;
	OCR2A = spd;
	OCR2B = spd;
	DDRD &= ~( 1 << PIND5);
	DDRB &= ~(1 << PINB3);
	DDRD |= (1 << PIND6);
	DDRD |= (1 << PIND3);
	
}

void L293D_moveLeft(){
	OCR0A = spd;
	OCR0B = spd;
	OCR2A = spd;
	OCR2B = spd;
	DDRD &= ~( 1 << PIND5);
	DDRB |= (1 << PINB3);
	DDRD &= ~(1 << PIND6);
	DDRD &= ~(1 << PIND3); 

}

void L293D_moveRight(){
	OCR0A = spd;
	OCR0B = spd;
	OCR2A = spd;
	OCR2B = spd;
	DDRD |= ( 1 << PIND5);
	DDRB &= ~((1 << PINB3)| (1 << PIND3));
	DDRD &= ~(1 << PIND6) ;
}

void L293_moveRightForward(){
	OCR0A =  spd ;  // Move at right
	OCR0B = spd;
	OCR2A = (spd * 75)/ 100;
	OCR2B = spd;
	DDRD |= ( 1 << PIND5);
	DDRB |= (1 << PINB3);
	DDRD &=  ~((1 << PIND6) | (1 << PIND3));
	
}

void L293_moveLeftForward(){
	OCR0A = (spd * 75)/ 100;
	OCR0B = spd;
	OCR2A =  spd;
	OCR2B = spd;
	DDRD |= ( 1 << PIND5);
	DDRB |= (1 << PINB3);
	DDRD &=  ~((1 << PIND6) | (1 << PIND3));
	
}
void L293D_stop(){
	DDRB &= ~(1 << PINB3);
	DDRD &= ~(1 << PIND5);
	DDRD &=  ~((1 << PIND6) | (1 << PIND3));
}