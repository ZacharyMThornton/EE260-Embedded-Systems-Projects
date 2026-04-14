//#include "sl_system_init.h"
#include "sl_sleeptimer.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "em_gpio.h"
#include "em_eusart.h"
#include "em_cmu.h"
#include "LL_UART.h"

// Sensor enable pins
#define SENSOR_ENABLE_PORT gpioPortC
#define SENSOR_ENABLE_PIN 9

// Define SPI pins (replace with actual port and pin numbers)
#define SPI_MOSI_PORT gpioPortC
#define SPI_MOSI_PIN 3
#define SPI_MISO_PORT gpioPortC
#define SPI_MISO_PIN 2
#define SPI_CLK_PORT gpioPortC
#define SPI_CLK_PIN 1
#define SPI_CS_PORT gpioPortA
#define SPI_CS_PIN 7

// SPI configuration
#define SPI_BITRATE     1000000  // 1 MHz

#define FSRCO_FREQ    (20000000UL) // 20 MHz

// Function to initialize SPI
void SPI_Init(void) {

  uint32_t bitRate = SPI_BITRATE;
  uint32_t clkdiv_HCLK;
  uint32_t clkdiv_PCLK;
  uint32_t sysclk = FSRCO_FREQ;
  uint32_t clkdiv;
  uint32_t refFreq;

  // Enable GPIO clock (in CLKEN0)
  CMU->CLKEN0_SET = CMU_CLKEN0_GPIO;

  // Enable EUSART1 clock (in CLKEN1)
  CMU->CLKEN1_SET = CMU_CLKEN1_EUSART1;

  // Enable SPI_MOSI in push-pull mode
  GPIO->P[SPI_MOSI_PORT].MODEL &= ~(0xF << (SPI_MOSI_PIN * 4));
  GPIO->P[SPI_MOSI_PORT].MODEL |= (0x4 << (SPI_MOSI_PIN * 4));
  // Set SPI_MOSI to low
  GPIO->P[SPI_MOSI_PORT].DOUT &= ~(1 << SPI_MOSI_PIN);

  // Enable SPI_MISO in input mode
  GPIO->P[SPI_MISO_PORT].MODEL &= ~(0xF << (SPI_MISO_PIN * 4));
  GPIO->P[SPI_MISO_PORT].MODEL |= (0x1 << (SPI_MISO_PIN * 4));
  // Set SPI_MISO to low
  GPIO->P[SPI_MISO_PORT].DOUT &= ~(1 << SPI_MISO_PIN);

  // Enable SPI_CLK in push-pull mode
  GPIO->P[SPI_CLK_PORT].MODEL &= ~(0xF << (SPI_CLK_PIN * 4));
  GPIO->P[SPI_CLK_PORT].MODEL |= (0x4 << (SPI_CLK_PIN * 4));
  // Set SPI_CLK to low
  GPIO->P[SPI_CLK_PORT].DOUT &= ~(1 << SPI_CLK_PIN);

  // Enable SPI_CS in push-pull mode
  GPIO->P[SPI_CS_PORT].MODEL &= ~(0xF << (SPI_CS_PIN * 4));
  GPIO->P[SPI_CS_PORT].MODEL |= (0x4 << (SPI_CS_PIN * 4));
  // Set SPI_CS to high
  GPIO->P[SPI_CS_PORT].DOUT |= (1 << SPI_CS_PIN);

  while ((EUSART1->SYNCBUSY & 0xFFF) != 0U) {
    // Waits for EUSART to be ready so we can modify the CFG registers
    // Do not remove
  }

  // Clear the CFG2 register with ~(0xFFFFFFFF)
  EUSART1->CFG2 &= ~(0xFFFFFFFF);

  // Set the Master enable and Auto CS enable bits of the CFG2 Register
  EUSART1->CFG2 |= (EUSART_CFG2_MASTER | EUSART_CFG2_AUTOCS);

  // Clear the CFG1 register with ~(0xFFFFFFFF)
  EUSART1->CFG1 &= ~(0xFFFFFFFF);

  // Clear the CFG0 register with ~(0xFFFFFFFF)
  EUSART1->CFG0 &= ~(0xFFFFFFFF);

  // Set the Sync Enable and MSB First Enable bits of the CFG0 Register
  EUSART1->CFG0 |= (EUSART_CFG0_SYNC | EUSART_CFG0_MSBF);

  // Clear TIMINGCFG Register with ~(0xFFFFFFFF)
  EUSART1->TIMINGCFG &= ~(0xFFFFFFFF);

  // Set SetupWindow bits of the TIMINGCFG register to 0x4
  EUSART1->TIMINGCFG |= (0x4 << _EUSART_TIMINGCFG_SETUPWINDOW_SHIFT);

  /*Prevent dividing by 0*/
  EFM_ASSERT(bitRate);

  // Get the HCLK divider from the CMU's SYSCLKCTRL register and add 0x1 to it
  clkdiv_HCLK = 1;

  // Get the PCLK divider from the CMU's SYSCLKCTRL register and add 0x1 to it
  clkdiv_PCLK = 1;

  // Calculate refFreq (aka PCLK) using sysclk, clkdiv_HCLK, and clkdiv_PCLK
  refFreq = sysclk / (clkdiv_HCLK * clkdiv_PCLK);

  // Calculate clkdiv
  clkdiv = refFreq / bitRate - 1UL;

  // Set the SDIV bits of the CFG2 register to clkdiv
  EUSART1->CFG2 |= (clkdiv << _EUSART_CFG2_SDIV_SHIFT);

  // Set the EN bit of the EN_SET register
  EUSART1->EN_SET = EUSART_EN_EN;

  while ((EUSART1->SYNCBUSY & 0xFFF) != 0U) {
    // Waits for EUSART to be ready so we can modify the CMD register
    // Do not remove
  }

  // Set the first 4 bits of the CMD register
  EUSART1->CMD |= 0xF;

  while ((EUSART1->SYNCBUSY
      & ((0x1 << 3) | (0x1 << 5) | (0x1 << 4) | (0x1 << 6))) != 0U) {
    // Waits for EUSART to be ready so we can modify the CMD register
    // Do not remove

  }

  // Wait for the status register to be updated.
  while ((EUSART1->STATUS & (0x2 | 0x1)) != ((0x1 << 0) | (0x1 << 1))) {
    // Waits for EUSART to be ready so we can modify the CMD register
    // Do not remove
  }

  while ((EUSART1->SYNCBUSY & (0x08 | 0x20)) != 0U) {
    // Waits for EUSART to be ready so we can modify the CMD register
    // Do not remove
  }

  // Set the first 2 bits of the CMD register
  EUSART1->CMD |= 0x3;

  while ((EUSART1->SYNCBUSY & (0x08 | 0x20)) != 0U) {
    // Waits for EUSART to be ready so we can modify the ROUTE register
    // Do not remove
  }

  while (~EUSART1->STATUS & (0x1000 | 0x2000)) {
    // Waits for EUSART to be ready so we can modify the ROUTE register
    // Do not remove
  }

  /* Enable pins at the correct EUSART location. */
  GPIO->EUSARTROUTE[1].TXROUTE = ((SPI_MOSI_PORT
          << _GPIO_EUSART_TXROUTE_PORT_SHIFT)
      | (SPI_MOSI_PIN << _GPIO_EUSART_TXROUTE_PIN_SHIFT));
  GPIO->EUSARTROUTE[1].RXROUTE = ((SPI_MISO_PORT
          << _GPIO_EUSART_RXROUTE_PORT_SHIFT)
      | (SPI_MISO_PIN << _GPIO_EUSART_RXROUTE_PIN_SHIFT));
  GPIO->EUSARTROUTE[1].SCLKROUTE = ((SPI_CLK_PORT
          << _GPIO_EUSART_SCLKROUTE_PORT_SHIFT)
      | (SPI_CLK_PIN << _GPIO_EUSART_SCLKROUTE_PIN_SHIFT));
  GPIO->EUSARTROUTE[1].ROUTEEN =
  GPIO_EUSART_ROUTEEN_RXPEN | GPIO_EUSART_ROUTEEN_TXPEN
      | GPIO_EUSART_ROUTEEN_SCLKPEN;
}
// Function to assert CS (active low)
void SPI_CS_Assert(void) {
  //Set SPI_CS to low
  GPIO->P[SPI_CS_PORT].DOUT &= ~(1 << SPI_CS_PIN);
}

// Function to deassert CS (inactive high)
void SPI_CS_Deassert(void) {
  //Set SPI_CS to High
  GPIO->P[SPI_CS_PORT].DOUT |= (1 << SPI_CS_PIN);
}

// Function to send and receive data over SPI
uint8_t SPI_Transfer(uint8_t data) {
  // Wait for the TXFL bit of the STATUS register to be set
  while (!(EUSART1->STATUS & EUSART_STATUS_TXFL))
    ;

  // Write the data to the TXDATA register
  EUSART1->TXDATA = data;

  // Wait for the RXFL bit of the STATUS register to be set
  while (!(EUSART1->STATUS & EUSART_STATUS_RXFL))
    ;

  // Return the data that was written to the RXDATA register
  return EUSART1->RXDATA;
}

// Example function to read a register from the IMU
uint8_t IMU_ReadRegister(uint8_t reg) {
  uint8_t value;

  // Assert the CS
  SPI_CS_Assert();
  // Call SPI_Transfer with the register address with the read bit set
  SPI_Transfer(reg | 0x80);
  // Call SPI_Transfer with 0x00 to read the data (returns uint8_t)
  value = SPI_Transfer(0x00);
  // Deassert the CS
  SPI_CS_Deassert();

  return value;
}

void IMU_ReadRegisterCont(uint8_t reg, uint8_t *buffer, uint8_t num) {

  // Assert the CS
  SPI_CS_Assert();
  // Call SPI_Transfer with the register address with the read bit set
  SPI_Transfer(reg | 0x80);
  while (num--) { //Don't change this
    *buffer++ = SPI_Transfer(0x00);
  }

  // Deassert the CS
  SPI_CS_Deassert();

}

// Example function to write a register to the IMU
void IMU_WriteRegister(uint8_t reg, uint8_t data) {
  // Assert the CS
  SPI_CS_Assert();
  // Call SPI_Transfer with the register address with the read bit cleared
  SPI_Transfer(reg & 0x7F);
  //Call SPI_Transfer with the data you want to send
  SPI_Transfer(data);
  // Deassert the CS
  SPI_CS_Deassert();
}

void IMU_WriteRegisterMasked(uint8_t reg, uint8_t value, uint8_t mask) {
  // Dont't touch this function
  uint8_t data;
  data = IMU_ReadRegister(reg);
  data &= ~(mask);
  data |= value;
  IMU_WriteRegister(reg, data);
}

// Function to initialize ICM-20689 ( onboard IMU )
void IMU_Init(void) {

  // Enable the GPIO clock (even if you already did previously)
  CMU->CLKEN0_SET = CMU_CLKEN0_GPIO;

  // Set SENSOR_ENABLE to push-pull usnig the MODEH register
  GPIO->P[SENSOR_ENABLE_PORT].MODEH &= ~(0xF << ((SENSOR_ENABLE_PIN - 8) * 4));
  GPIO->P[SENSOR_ENABLE_PORT].MODEH |= (0x4 << ((SENSOR_ENABLE_PIN - 8) * 4));

  // Set SENSOR_ENABLE high
  GPIO->P[SENSOR_ENABLE_PORT].DOUT |= (1 << SENSOR_ENABLE_PIN);

  /*IMU start up procedure, do not touch any of this*/
  IMU_WriteRegister(0x6B, 0x01);
  IMU_WriteRegister(0x6B, 0x80);
  sl_sleeptimer_delay_millisecond(100);
  IMU_WriteRegister(0x6B, 0x01);
  while (IMU_ReadRegister(0x75) != 0x98) {
    USART0_sendString("IMU Startup Failure\r\n");
    sl_sleeptimer_delay_millisecond(10000);
  }
  IMU_WriteRegister(0x6C, 0x00);
  IMU_WriteRegister(0x1C, 0x18);
  IMU_WriteRegister(0x1B, 0x18);
  IMU_WriteRegister(0x1D, 0x08);
  IMU_WriteRegister(0x1A, 0x00);
  IMU_WriteRegister(0x19, 0x00);
}
void IMU_ReadAccel(int16_t *buffer) {
  /*Reads the Accelerometer data, do not touch any of this*/
  uint8_t temp[6];
  IMU_ReadRegisterCont(0x3B, temp, 6);
  while (!(IMU_ReadRegister(0x3A) & 0x01))
    ;

  *(buffer) = ((((int16_t) *(temp + 0)) << 8) | (*(temp + 1)));
  *(buffer + 1) = ((((int16_t) *(temp + 2)) << 8) | (*(temp + 3)));
  *(buffer + 2) = ((((int16_t) *(temp + 4)) << 8) | (*(temp + 5)));

}
void IMU_ReadGyro(int16_t *buffer) {
  /*Reads the Gyro data, do not touch any of this*/
  uint8_t temp[6];
  IMU_ReadRegisterCont(0x43, temp, 6);
  while (!(IMU_ReadRegister(0x3A) & 0x01))
    ;

  *(buffer) = ((((int16_t) *(temp + 0)) << 8) | (*(temp + 1)));
  *(buffer + 1) = ((((int16_t) *(temp + 2)) << 8) | (*(temp + 3)));
  *(buffer + 2) = ((((int16_t) *(temp + 4)) << 8) | (*(temp + 5)));

}

void IMU_DataOut(int16_t *buffer) {
  /*Outputs the float values of the IMU data to the serial console, do not touch any of this
   *For this to work you need to have -u_printf_float as a linker option (see lab6.docx)*/
  float data[6];
  char buf[100];

  *(data) = *(buffer) * (16 / 32767.5);
  *(data + 1) = *(buffer + 1) * (16 / 32767.5);
  *(data + 2) = *(buffer + 2) * (16 / 32767.5);
  *(data + 3) = *(buffer + 3) * (2000 / 32767.5);
  *(data + 4) = *(buffer + 4) * (2000 / 32767.5);
  *(data + 5) = *(buffer + 5) * (2000 / 32767.5);

  snprintf(buf, sizeof(buf), "%0.2f", *(data));
  USART0_sendString("\r\nXacc: ");
  USART0_sendString(buf);
  snprintf(buf, sizeof(buf), "%0.2f", *(data + 1));
  USART0_sendString("\r\nYacc: ");
  USART0_sendString(buf);
  snprintf(buf, sizeof(buf), "%0.2f", *(data + 2));
  USART0_sendString("\r\nZacc: ");
  USART0_sendString(buf);
  snprintf(buf, sizeof(buf), "%0.2f", *(data + 3));
  USART0_sendString("\r\nXgyro: ");
  USART0_sendString(buf);
  snprintf(buf, sizeof(buf), "%0.2f", *(data + 4));
  USART0_sendString("\r\nYgyro: ");
  USART0_sendString(buf);
  snprintf(buf, sizeof(buf), "%0.2f", *(data + 5));
  USART0_sendString("\r\nZgyro: ");
  USART0_sendString(buf);
}

int main(void) {
  sl_sleeptimer_init();
  initUSART0(115200); // Note, calling sl_system_init() breaks USART so do not call it
  SPI_Init();
  IMU_Init();
  sl_sleeptimer_delay_millisecond(100);
  USART0_sendString("Setup Complete\r\n");

  // Set up the on-board LED (any color you want)
  GPIO_PinModeSet(gpioPortA, 4, gpioModePushPull, 0);

  int16_t buffer[6];

  while (1) {
    while (IMU_ReadRegister(0x75) == 0x98) {
      IMU_ReadAccel(buffer);
      IMU_ReadGyro(buffer + 3);

      // Turn the LED off
      GPIO->P[gpioPortA].DOUT &= ~(1 << 4);

      IMU_DataOut(buffer);
      USART0_sendString("\r\n");

      // Turn the LED on
      GPIO->P[gpioPortA].DOUT |= (1 << 4);

      // The LED will flicker slightly instead of turning off entirely, this is intentional
      sl_sleeptimer_delay_millisecond(100);

    }

    while (IMU_ReadRegister(0x75) != 0x98) {
      USART0_sendString(
          "Something has gone wrong if you're\r\nseeing this in the serial terminal\r\n");
      sl_sleeptimer_delay_millisecond(10000);
    }
  }

}
