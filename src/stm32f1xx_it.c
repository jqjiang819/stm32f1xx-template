#include "stm32f1xx_it.h"
#include "usbd_core.h"

extern usbd_device usbd;

void NMI_Handler() {
    while (1) {}
}

void HardFault_Handler() {
    while (1) {}
}

void MemManage_Handler() {
    while (1) {}
}

void BusFault_Handler() {
    while (1) {}
}

void UsageFault_Handler() {
    while (1) {}
}

void SVC_Handler() {
}

void DebugMon_Handler() {
}

void PendSV_Handler() {
}

void SysTick_Handler() {
    HAL_IncTick();
}

void USB_LP_CAN1_RX0_IRQHandler() {
    usbd_poll(&usbd);
}

void USBWakeUp_IRQHandler() {
    usbd_poll(&usbd);
}
