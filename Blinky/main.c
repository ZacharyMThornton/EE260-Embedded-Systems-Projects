#include "em_cmu.h"
#include "em_gpio.h"
#include "sl_sleeptimer.h"

int main(void)
{
    sl_sleeptimer_init();

    CMU->CLKEN0 |= 0x1 << 26; // enable GPIO clock

    GPIO->P[gpioPortD].MODEL |= 0x4 << (4 * 2); // set LED red as push pull

    while (1)
    {
        GPIO->P[gpioPortD].DOUT &= ~(1 << 2); // turn on LED by clearing bit
        sl_sleeptimer_delay_millisecond(500); // delay 500 ms
        GPIO->P[gpioPortD].DOUT |= 1 << 2;    // turn off LED by setting bit
        sl_sleeptimer_delay_millisecond(500); // delay 500 ms
    }
}
