#ifndef LL_UART_H_
#define LL_UART_H_

void initUSART0(uint32_t baudrate);
void USART0_sendChar(const char c);
void USART0_sendString(char *c);
char USART0_receiveChar(void);

#endif /* LL_UART_H_ */
