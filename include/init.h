#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

#define LED_GPIO GPIOC
#define LED_PIN  GPIO_PIN_13

void Error_Handler();

void SystemClock_Init();
void GPIO_Init();

#ifdef __cplusplus
}
#endif
