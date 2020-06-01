#include "USART.h"
#include "DCMTR.h"
#include <avr/io.h>
#include <avr/interrupt.h>
// Note asynchronous (This library specially written for this application)
//  RC CAR with 4 directions and ons side obstacle detection and stop

//The library uses Receive interrupt to control the RC car


volatile unsigned char returned = '0';

unsigned long distance;   //mandatory to be long to give correct sensor distance reading
unsigned char Stop_flag  = 1;


//initalize
void USART_init(int BaudRate , int FOSC){
	UCSR0A  = (1 << U2X0);
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0) ;
	
	/* This was for synchronous operation only */
	//UCSRC = (1 << UCSZ0) | (1 << UCSZ1)|(1 << URSEL);
	//int br = FOSC / (16 * BaudRate) - 1;
	UBRR0L = 12;
	UBRR0H = 0;
}
/*
https://embeddedfreak.wordpress.com/2008/10/01/avr-udrie-vs-txc-interrupt/
UDRE is normally used. TXC is normally not used, except for these situations:

Half duplex communication, such as RS485: where the application needs to know whether all of the bits has been transmitted or not, before releasing the bus.
Power saving mode: where the application needs to know whether the transmission has done, before switching to deeper power saving mode (where the UART peripheral is not available anymore).
*/

// Receive interrupt if the micro controller raises an interrupt that data is ready
// this will execute 
ISR(USART_RX_vect)   // must be written as RX not RXC
{
	   returned = UDR0;   // returned   <- UDR0
	   unsigned char c = returned;    // C -> returned
	   if( distance > 30 ){    // if motor farther than the threshold distance 
	   if(c == 'M')           //states for motor speed
	   L293D_setSpeed(UDR0);  // Set speed based on the second value  
	   else if(c == 'F'){      // If forward is pressed send forward
	   L293D_moveForward();
	   Stop_flag = 0;
	   }
	   else if(c == 'B'){
	   L293D_moveBackward();
	   Stop_flag = 0;
	   }
	   else if(c  == 'R'){
	   L293D_moveRight();
	   Stop_flag = 0;
	   }
	   else if(c == 'L'){
	   L293D_moveLeft();
	   Stop_flag = 0;
	   }
	   else if(c == 'S'){
	   L293D_stop();
	   Stop_flag = 1;
	     }
		else if(c == 'I'){
		L293_moveLeftForward();
		Stop_flag == 0;
		}
		else if(c == 'E'){
		L293_moveLeftForward();
		Stop_flag == 0;
		}
	   }
	   else{   // else Stop
		  L293D_stop(); 
	   }
	//USART_send(returned);	
}
void USART_send(unsigned char c){            //Used polling not interrupt
	while(!(UCSR0A & (1 << UDRE0)));         // wait for the previous character to be sent 
	UDR0 = c;                                // Send this character  
}

/* This used initially to be used in main while(1) loop to take readings
   but proved to fail for no reason*/ 
/*
unsigned char USART_recieve(){
	unsigned char to_be_returned = returned;
	returned = '0';
	return to_be_returned;	
}
*/
void USART_sendString(char *S){
    int i = 0;
	while(S[i] != '\0'){
		USART_send(S[i]);
		i++;
	}	
}