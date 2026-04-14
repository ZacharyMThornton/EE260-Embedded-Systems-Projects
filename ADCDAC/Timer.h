#ifndef TIMER_H_
#define TIMER_H_
#include "em_timer.h"
#include "em_cmu.h"
#include <stdint.h>

// Desired Frequency in Hz
#define PWM_FREQ 50

// Define which pin the PWM will be output on
#define PWM_PORT gpioPortA
#define PWM_PIN 6

void timer_init(int16_t Port, int16_t Pin, uint16_t Hz);
void timer_init_OF();
void setDutyCycle(float dutyCycle);
void changeFrequency(uint16_t Hz);

#endif /* TIMER_H_ */
