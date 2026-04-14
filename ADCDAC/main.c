#include "Timer.h"

#include "em_cmu.h"
#include "em_gpio.h"
#include "em_iadc.h" // ADC is called "incremental ADC" in EFR32, software component must be installed
#include "em_vdac.h"
#include "sl_sleeptimer.h"
#include "LL_UART.h"
#include <stdio.h>

// ADC clock selection
#define CLK_SRC_ADC_FREQ 20000000 // 20 MHz
#define CLK_ADC_FREQ 16000000 // 16 MHz

// Modifying these only really becomes necessary when using multiple ADC channels at once
#define IADC_INPUT_0_PORT_PIN iadcPosInputPortBPin3 // physical pin to map ADC channel to
#define IADC_INPUT_0_BUS BBUSALLOC // Analog bus for this mapping
#define IADC_INPUT_0_BUSALLOC GPIO_BBUSALLOC_BODD0_ADC0 // odd pin 0

// VDAC defines
#define CHANNEL_NUM 0
#define CLK_VDAC_FREQ 1000000 // max frequency 1MHz
#define VDAC_REF 3300 // mv, determined by config

static void initVDAC(void);
static uint32_t get_vdac_value(uint32_t mv, uint32_t vref);

static volatile IADC_Result_t sample; // Raw IADC conversion result
static volatile uint32_t singleResult; // Result converted to millivolts

char buf[100];

static void initADC(void);

volatile bool run = false;


void TIMER0_IRQHandler(void){
  // check if the timer flag is set
  if (TIMER0->IF & TIMER_IF_OF){
      // clear the interrupt flag
      TIMER0->IF_CLR = TIMER_IF_OF;

      run = true;

  }
}

int main(void) {
    sl_sleeptimer_init();

    CMU_ClockEnable(cmuClock_GPIO, true); // enable GPIO clock
    CMU_ClockEnable(cmuClock_IADC0, true); // enable ADC clock
    CMU_ClockEnable(cmuClock_VDAC0, true); // enable DAC clock

    initUSART0(115200);

    initADC();
    initVDAC();

    timer_init_OF(5); // 5 Hz is 0.2 seconds

    while(1) {
        if (run){
            IADC_command(IADC0, iadcCmdStartSingle); // run ADC sampling
            sample = IADC_pullSingleFifoResult(IADC0); // get ADC sample result
            singleResult = sample.data * 3300 / 0xFFF; // mv = sample * (max_mv/max_sample)

            USART0_sendString("ADC sample: ");
            snprintf(buf, sizeof(buf), "%lu", sample.data);
            USART0_sendString(buf);
            USART0_sendString("/4095 ADC voltage: ");
            snprintf(buf, sizeof(buf), "%lu", singleResult);
            USART0_sendString(buf);
            USART0_sendString("\r\n");

            VDAC_ChannelOutputSet(VDAC0, CHANNEL_NUM, get_vdac_value(singleResult, VDAC_REF));
            run = false;
        }

    }
}

static void initADC(void) {
    // Use single input (referenced to GND) as opposed to differential
    IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;
    IADC_SingleInput_t singleInput = IADC_SINGLEINPUT_DEFAULT;
    singleInput.posInput = IADC_INPUT_0_PORT_PIN;
    singleInput.negInput = iadcNegInputGnd;

    // prescale source clock
    IADC_Init_t init = IADC_INIT_DEFAULT;
    init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0);
    init.warmup = iadcWarmupNormal;

    IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;
    initAllConfigs.configs[0].reference = iadcCfgReferenceVddx;
    initAllConfigs.configs[0].vRef = 3300;
    initAllConfigs.configs[0].osrHighSpeed = iadcCfgOsrHighSpeed2x;
    initAllConfigs.configs[0].analogGain = iadcCfgAnalogGain1x;

    // prescale ADC clock to 16MHz
    initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0,
                                                                       CLK_ADC_FREQ,
                                                                       0,
                                                                       iadcCfgModeNormal,
                                                                       init.srcClkPrescale);

    GPIO->IADC_INPUT_0_BUS |= IADC_INPUT_0_BUSALLOC; // Allocate analog bus for ADC inputs

    IADC_init(IADC0, &init, &initAllConfigs);
    IADC_initSingle(IADC0, &initSingle, &singleInput);
}

static void initVDAC(void)
{
  VDAC_Init_TypeDef init = VDAC_INIT_DEFAULT;
  VDAC_InitChannel_TypeDef initChannel = VDAC_INITCHANNEL_DEFAULT;

  // Calculate the VDAC clock prescaler value resulting in a 1 MHz VDAC clock
  init.prescaler = VDAC_PrescaleCalc(VDAC0, CLK_VDAC_FREQ);
  init.onDemandClk = false; // Clocking is requested on demand
  init.reference = vdacRefAvdd; // alternatively, 2V5 or Avdd

  initChannel.highCapLoadEnable = false; // Disable High Capacitance Load mode
  initChannel.powerMode = vdacPowerModeLowPower; // Use Low Power mode
  initChannel.port = vdacChPortD;
  initChannel.pin = 2;

  VDAC_Init(VDAC0, &init);
  VDAC_InitChannel(VDAC0, &initChannel, CHANNEL_NUM);

  VDAC_Enable(VDAC0, CHANNEL_NUM, true);
}

static uint32_t get_vdac_value(uint32_t mv, uint32_t vref)
{
  return (uint32_t)((mv / (float)vref) * 0xFFF);
}






