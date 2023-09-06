#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

#define LED_GPIO GPIOC
#define LED_PIN  GPIO_PIN_13

typedef void (*cdc_msg_callback_t)(uint8_t* data, uint16_t len);

void Error_Handler();

void SystemClock_Init();
void GPIO_Init();

void USBCDC_Init();
void USBCDC_Transmit(uint8_t* data, uint16_t len);
void USBCDC_SetReceiveCallback(cdc_msg_callback_t callback);

#ifdef __cplusplus
}
#endif
