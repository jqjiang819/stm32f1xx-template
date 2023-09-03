#include "init.h"

int main() {
    HAL_Init();

    SystemClock_Init();
    GPIO_Init();

    while (1) {
        HAL_GPIO_TogglePin(LED_GPIO, LED_PIN);
        HAL_Delay(1000);
    }
}
