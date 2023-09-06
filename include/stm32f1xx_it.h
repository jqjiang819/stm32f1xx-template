#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/

/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler();

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler();

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler();

/**
 * @brief This function handles Prefetch fault, memory access fault.
 */
void BusFault_Handler();

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler();

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler();

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler();

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler();

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler();

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles USB low priority or CAN RX0 interrupts.
 */
void USB_LP_CAN1_RX0_IRQHandler();

/**
 * @brief This function handles USB wake up interrupts.
 */
void USBWakeUp_IRQHandler();

#ifdef __cplusplus
}
#endif
