

extern unsigned long distance;   //mandatory to be long to give correct sensor distance reading
extern unsigned char Stop_flag ;

void USART_init(int BaudRate , int FOSC);
void USART_send(unsigned char c);
unsigned char  USART_recieve();
void USART_sendString(char *S);