#include "em_cmu.h"
#include "em_gpio.h"
#include "sl_sleeptimer.h"
#include <string.h>

#define DATA_PORT gpioPortC
#define D7_PIN 4
#define D6_PIN 3
#define D5_PIN 2
#define D4_PIN 1

#define CTL_PORT gpioPortA
#define RS_PIN 5
#define RW_PIN 6
#define EN_PIN 7

void clear_model();

void LCD_init();
void LCD_send_byte(uint8_t data, uint8_t RS);
void LCD_send_nibble(uint8_t nibble);



int main(void){

  CMU->CLKEN0 |= 0x1 << 26; // enable GPIO clock
  sl_sleeptimer_init();

  // set control pins to push-pull
  GPIO->P[CTL_PORT].MODEL |=  (0x4 << (RS_PIN * 4));
  GPIO->P[CTL_PORT].MODEL |=  (0x4 << (RW_PIN * 4));
  GPIO->P[CTL_PORT].MODEL |=  (0x4 << (EN_PIN * 4));

  // set RW to 0 to write
  GPIO->P_CLR[CTL_PORT].DOUT = 1 << RW_PIN;

  LCD_init();

  char *word = "I'm a Robot!";
  char *word2 = "Beep Beep";

  while (1) {

    LCD_send_byte(0x01, 0);   // Clear display
    sl_sleeptimer_delay_millisecond(2);

    LCD_send_byte(0x80, 0); // DDRAM address 0 (line 1)
    sl_sleeptimer_delay_millisecond(1);

    for (size_t i = 0; i < strlen(word); i++) {
      LCD_send_byte(*(word + i), 1);
    }
    sl_sleeptimer_delay_millisecond(2000);


//    LCD_send_byte(0x01, 0);   // Clear display
//    sl_sleeptimer_delay_millisecond(2);

    LCD_send_byte(0xC0, 0); // DDRAM address 0 (line 1)
    sl_sleeptimer_delay_millisecond(1);

    for (size_t i = 0; i < strlen(word2); i++) {
      LCD_send_byte(*(word2 + i), 1);
    }
    sl_sleeptimer_delay_millisecond(2000);
   }

}

void clear_model(){
  // clear previous modes
  GPIO->P[DATA_PORT].MODEL &= ~(0xf << 4 * D7_PIN);
  GPIO->P[DATA_PORT].MODEL &= ~(0xf << 4 * D6_PIN);
  GPIO->P[DATA_PORT].MODEL &= ~(0xf << 4 * D5_PIN);
  GPIO->P[DATA_PORT].MODEL &= ~(0xf << 4 * D4_PIN);
}

void LCD_send_byte(uint8_t data, uint8_t RS){

  if (RS){
      GPIO->P_SET[CTL_PORT].DOUT = 1 << RS_PIN;
  }else{
      GPIO->P_CLR[CTL_PORT].DOUT = 1 << RS_PIN;
  }

  // set pins to upper byte
  GPIO->P_CLR[DATA_PORT].DOUT = 0xf << D4_PIN;
  GPIO->P_SET[DATA_PORT].DOUT = ((data >> 4) & 0x0F) << D4_PIN;

  // pulse enable
  GPIO->P_SET[CTL_PORT].DOUT = 1 << EN_PIN;
  sl_sleeptimer_delay_millisecond(1);
  GPIO->P_CLR[CTL_PORT].DOUT = 1 << EN_PIN;
  sl_sleeptimer_delay_millisecond(1);

  // set pins to lower byte
  GPIO->P_CLR[DATA_PORT].DOUT = 0xf << D4_PIN;
  GPIO->P_SET[DATA_PORT].DOUT = (data & 0x0F) << D4_PIN;

  // pulse enable
  GPIO->P_SET[CTL_PORT].DOUT = 1 << EN_PIN;
  sl_sleeptimer_delay_millisecond(1);
  GPIO->P_CLR[CTL_PORT].DOUT = 1 << EN_PIN;
  sl_sleeptimer_delay_millisecond(1);


}

void LCD_send_nibble(uint8_t nibble){
  // set RS to 0
  GPIO->P_CLR[CTL_PORT].DOUT = 1 << RS_PIN;

  // clear data pins
  // Clear data pins first, then set according to nibble
  GPIO->P_CLR[DATA_PORT].DOUT = (1 << D4_PIN) | (1 << D5_PIN) | (1 << D6_PIN) | (1 << D7_PIN);

  // Set pins according to nibble bits
  if (nibble & 0x01) GPIO->P_SET[DATA_PORT].DOUT = 1 << D4_PIN;
  if (nibble & 0x02) GPIO->P_SET[DATA_PORT].DOUT = 1 << D5_PIN;
  if (nibble & 0x04) GPIO->P_SET[DATA_PORT].DOUT = 1 << D6_PIN;
  if (nibble & 0x08) GPIO->P_SET[DATA_PORT].DOUT = 1 << D7_PIN;

  //pulse enable;
  GPIO->P_SET[CTL_PORT].DOUT = 1 << EN_PIN;
  sl_sleeptimer_delay_millisecond(1);
  GPIO->P_CLR[CTL_PORT].DOUT = 1 << EN_PIN;
  sl_sleeptimer_delay_millisecond(1);

}

void LCD_init(){
  clear_model();

  // clear data pins
  GPIO->P_CLR[DATA_PORT].DOUT = 0xf << D4_PIN;

  // set data pins to push-pull
  GPIO->P[DATA_PORT].MODEL |= 0x4 << 4 * D7_PIN;
  GPIO->P[DATA_PORT].MODEL |= 0x4 << 4 * D6_PIN;
  GPIO->P[DATA_PORT].MODEL |= 0x4 << 4 * D5_PIN;
  GPIO->P[DATA_PORT].MODEL |= 0x4 << 4 * D4_PIN;

  // initialization sequence
  sl_sleeptimer_delay_millisecond(40);
  LCD_send_nibble(0x3);
  sl_sleeptimer_delay_millisecond(5);
  LCD_send_nibble(0x3);
  sl_sleeptimer_delay_millisecond(5);
  LCD_send_nibble(0x3);
  sl_sleeptimer_delay_millisecond(5);
  LCD_send_nibble(0x2);
  sl_sleeptimer_delay_millisecond(5);

  // send initialization instructions
  LCD_send_byte(0x28, 0);  // Function set: 4-bit, 2 lines, 5x8 font
  sl_sleeptimer_delay_millisecond(1);
  LCD_send_byte(0x08, 0);  // Display off
  sl_sleeptimer_delay_millisecond(1);
  LCD_send_byte(0x01, 0);  // Clear display
  sl_sleeptimer_delay_millisecond(2);
  LCD_send_byte(0x06, 0);  // Entry mode: increment, no shift
  sl_sleeptimer_delay_millisecond(1);
  LCD_send_byte(0x0C, 0);  // Display on, cursor off, blink off
  sl_sleeptimer_delay_millisecond(1);

}
