#include "esp_partition.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    while (1) {
        printf("hello\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}