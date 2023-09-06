#include "init.h"

void OnReceiveMsg(uint8_t* buf, uint16_t len) {
    USBCDC_Transmit(buf, len);
}

int main() {
    HAL_Init();

    SystemClock_Init();
    GPIO_Init();
    USBCDC_Init();

    USBCDC_SetReceiveCallback(OnReceiveMsg);

    while (1) {
        HAL_GPIO_TogglePin(LED_GPIO, LED_PIN);
        HAL_Delay(1000);
    }
}
