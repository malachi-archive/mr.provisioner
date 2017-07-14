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


void coap_server_task(void* pvParameters);

// DTLS code needs up to 4k, they say
// TODO: Whiddle this down
#define COAP_SERVER_STACK   4096

//int main()
extern "C" int app_main()
{
    xTaskCreate(blinkenTask, "blinkenTask", 512, NULL, 2, NULL);
    xTaskCreate(coap_server_task, "coapServer", COAP_SERVER_STACK, NULL, 2, NULL);

    return 0;
}