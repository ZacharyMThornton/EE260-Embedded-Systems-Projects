#include "Timer.h"

void timer_init(int16_t Port, int16_t Pin, uint16_t Hz) {

  // Enable the GPIO Clock and TIMER0 Clock
  CMU->CLKEN0 |= ((0x1 << 26) | (0x1 << 4));

  // Configure Output Pin as Push-Pull
  GPIO->P[Port].MODEL |= 0x4 << (4 * Pin);

  // Initialize some needed variables for configuration
  uint32_t timerFreq, topValue;
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;

  // Don't start counter on initialization
  timerInit.enable = false;

  // PWM mode sets/clears the output on compare/overflow events
  timerCCInit.mode = timerCCModePWM;

  //Please open this functions deceleration by pressing F3 while your cursor is hovering over it
  //Its important to understand what its doing
  TIMER_Init(TIMER0, &timerInit);

  // Route CC0 output to PA6 using the TIMERROUTE[0].ROUTEEN and TIMERROUTE[0].CC0ROUTE registers
  GPIO->TIMERROUTE[0].ROUTEEN = GPIO_TIMER_ROUTEEN_CC0PEN;
  GPIO->TIMERROUTE[0].CC0ROUTE = (Port << _GPIO_TIMER_CC0ROUTE_PORT_SHIFT)
      | (Pin << _GPIO_TIMER_CC0ROUTE_PIN_SHIFT);

  //Again Please open this functions deceleration by pressing F3 while your cursor is hovering over it
  //Its important to understand what its doing
  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  // Set top value to overflow at the desired PWM_FREQ frequency
  // You may use CMU_ClockFreqGet() and the cmuClock_TIMER0 macro to do this
  timerFreq = CMU_ClockFreqGet(cmuClock_TIMER0) / (timerInit.prescale + 1);

  // Calculate topValue based on timerFreq and PWM_FREQ
  topValue = (timerFreq / Hz);

  // Set the TOP register of TIMER0 to topValue
  TIMER0->TOP = topValue;

  // Enable TIMER0's Routing Interrupt using the IEN register, you may use the TIMER_IEN_CC0 macro
  // It may be enabled already so make sure you use the correct assignment operator
  TIMER0->IEN |= TIMER_IEN_CC0;
  NVIC_EnableIRQ(TIMER0_IRQn); // NEW

  // Start TIMER0 using the CMD register, you may use the TIMER_CMD_START macro to do this
  TIMER0->CMD = TIMER_CMD_START;

}

void timer_init_OF(int Hz){
  // Enable the GPIO Clock and TIMER0 Clock
  CMU->CLKEN0 |= ((0x1 << 26) | (0x1 << 4));

  // Initialize some needed variables for configuration
  uint32_t timerFreq, topValue;
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;

  // Don't start counter on initialization
  timerInit.enable = false;

  //Please open this functions deceleration by pressing F3 while your cursor is hovering over it
  //Its important to understand what its doing
  TIMER_Init(TIMER0, &timerInit);

  // Set top value to overflow at the desired PWM_FREQ frequency
  // You may use CMU_ClockFreqGet() and the cmuClock_TIMER0 macro to do this
  timerFreq = CMU_ClockFreqGet(cmuClock_TIMER0) / (timerInit.prescale + 1);

  // Calculate topValue based on timerFreq and PWM_FREQ
  topValue = (timerFreq / Hz);

  // Set the TOP register of TIMER0 to topValue
  TIMER0->TOP = topValue;

  // Enable TIMER0's Routing Interrupt using the IEN register, you may use the TIMER_IEN_CC0 macro
  // It may be enabled already so make sure you use the correct assignment operator
  TIMER0->IEN |= TIMER_IEN_OF;
  NVIC_EnableIRQ(TIMER0_IRQn); // NEW

  // Start TIMER0 using the CMD register, you may use the TIMER_CMD_START macro to do this
  TIMER0->CMD = TIMER_CMD_START;
}

void setDutyCycle(float dutyCycle) {
  uint32_t topValue, compareValue;
  // Figure out the top value of TIMER0 using the TOP register
  topValue = TIMER0->TOP;

  // Calculate compareValue based on topValue and dutyCycle
  // make sure to cast your result to uint32_t
  compareValue = (uint32_t) ((topValue * dutyCycle) / 100);

  // Set the compare value for TIMER0 using the CC[0].OBC register
  TIMER0->CC[0].OCB = compareValue;

}

void changeFrequency(uint16_t Hz) {

  // Initialize some needed variables for configuration
  uint32_t timerFreq, topValue, compareValue;
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;

  timerFreq = CMU_ClockFreqGet(cmuClock_TIMER0) / (timerInit.prescale + 1);

  // Calculate topValue based on timerFreq and Hz
  topValue = (timerFreq / Hz);
  compareValue = (uint32_t) ((topValue * 50) / 100);

  while (TIMER0->CNT > topValue - 1000) // DO NOT CHANGE THIS YOUR CODE WILL BREAK
    TIMER_SyncWait(TIMER0);

  // Set the TOP register of TIMER0 to topValue
  TIMER0->TOP = topValue;

  // Set the compare value for TIMER0 using the CC[0].OBC register

}
