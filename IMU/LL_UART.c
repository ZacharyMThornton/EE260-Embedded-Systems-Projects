#include "em_device.h"
#include "em_chip.h"
#include "em_usart.h"
#include "em_cmu.h"
#include "sl_sleeptimer.h"
#include "LL_UART.h"
#include <string.h>

#define UART_TX_PIN       5 // Find the proper pin number of the TX line
#define UART_RX_PIN       6 // FInd the proper pin number of the RX line
#define UART_PORT         gpioPortA // Find the proper port of UART0
#define CLKDIV_MASK  _USART_CLKDIV_DIV_MASK
#define FSRCO_FREQ    (20000000UL)

void initUSART0(uint32_t baudrate) {
  // Enable clock for GPIO and USART0
  CMU->CLKEN0 |= ((0x1 << 26) | (0x1 << 9)); // Enable GPIO clock

  // Clear the TX pin and RX pin mode register
  GPIO->P[UART_PORT].MODEL &= (~(0xF << (4 * UART_TX_PIN)))
      | (~(0xF << (4 * UART_RX_PIN))); // Clear existing mode for TX pin

  // Configure TX pin as output and RX pin as input
  GPIO->P[UART_PORT].MODEL |= (0x4 << (4 * UART_TX_PIN))
      | (0x1 << (4 * UART_RX_PIN)); // Set TX pin to push-pull output

  // Configure USART0 for basic asynchronous operation
  USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT; // Default initialization structure

  init.baudrate = baudrate; // Set baud rate to 115200

  USART0->EN_SET = USART_EN_EN;

  /* Configure databits, stopbits, and parity. */
  USART0->FRAME = init.databits | init.stopbits
      | init.parity;

  /* Configure baudrate. */
  uint32_t clkdiv_HCLK;
  uint32_t clkdiv_PCLK;
  uint32_t sysclk = FSRCO_FREQ;
  uint32_t clkdiv;
  uint32_t refFreq = 0;
  uint32_t oversample = 0;

  /* Inhibit divide by 0 */
  EFM_ASSERT(baudrate);

  clkdiv_HCLK = ((CMU->SYSCLKCTRL >> 12) & (0xF)) + 0x1;
  clkdiv_PCLK = ((CMU->SYSCLKCTRL >> 10) & (0x1)) + 0x1;
  refFreq = (sysclk / clkdiv_HCLK) / clkdiv_PCLK;

  //SystemSYSCLKGet();
  /* Map oversampling. */
  switch (init.oversampling) {
  case usartOVS16:
    EFM_ASSERT(baudrate <= (refFreq / 16));
    oversample = 16;
    break;

  case usartOVS8:
    EFM_ASSERT(baudrate <= (refFreq / 8));
    oversample = 8;
    break;

  case usartOVS6:
    EFM_ASSERT(baudrate <= (refFreq / 6));
    oversample = 6;
    break;

  case usartOVS4:
    EFM_ASSERT(baudrate <= (refFreq / 4));
    oversample = 4;
    break;

  default:
    /* Invalid input */
    EFM_ASSERT(0);
    break;
  }

  clkdiv = 32 * refFreq + (oversample * baudrate) / 2;
  clkdiv /= oversample * baudrate;
  clkdiv -= 32;
  clkdiv *= 8;

  /* Make sure that reserved bits are not written to. */
  clkdiv &= CLKDIV_MASK;

  USART0->CTRL &= ~_USART_CTRL_OVS_MASK;
  USART0->CTRL |= init.oversampling;
  USART0->CLKDIV = clkdiv;

  /* Finally, enable (as specified). */
  USART0->CMD = (uint32_t) init.enable;

  // Route USART0 TX and RX to the defined pins
  GPIO->USARTROUTE[0].TXROUTE = (UART_PORT << _GPIO_USART_TXROUTE_PORT_SHIFT)
      | (UART_TX_PIN << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].RXROUTE = (UART_PORT << _GPIO_USART_RXROUTE_PORT_SHIFT)
      | (UART_RX_PIN << _GPIO_USART_RXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].ROUTEEN = GPIO_USART_ROUTEEN_TXPEN
      | GPIO_USART_ROUTEEN_RXPEN;  // Enable TX and RX routing
}

void USART0_sendChar(const char c) {
  // Send a single byte
  while (!(USART0->STATUS & USART_STATUS_TXBL))
    ;
  USART0->TXDATA = c;

}

void USART0_sendString(char *c) {
  // Loop through all values of string using a for loop
  // string lenght can be found by doing strlen(c)
  // Inside the for loop call USART0_sendChar(c[i])
  for (size_t i = 0; i < strlen(c); i++) {
    USART0_sendChar(*(c + i));
  }

}

char USART0_receiveChar(void) {
  // Wait until the recieved buffer has data
  while (!(USART0->STATUS & USART_STATUS_RXDATAV))
    ;
  return USART0->RXDATA;

}
