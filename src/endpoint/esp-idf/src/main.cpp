#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void blinkenTask(void *pvParameters)
{
    //gpio_enable(gpio, GPIO_OUTPUT);
    while(1) {
        //gpio_write(gpio, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        //gpio_write(gpio, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("Hello World\r\n");
    }
}
//int main()
extern "C" int app_main()
{
    xTaskCreate(blinkenTask, "blinkenTask", 1024, NULL, 2, NULL);

    return 0;
}