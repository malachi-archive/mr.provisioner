extern "C" {

#include <FreeRTOS.h>
#include <task.h>
#include <esp/uart.h>

#ifndef ENABLE_AP

#include "espressif/esp_common.h"
#include <espressif/esp_sta.h>
#include <espressif/esp_wifi.h>
#endif

}

#include "ssid_config.h"

void setup_coap();
void setup_ap();



extern "C" void user_init(void)
{
    uart_set_baud(0, 115200);

#ifndef ENABLE_AP
    struct sdk_station_config config = {
        WIFI_SSID,
        WIFI_PASS,
    };

    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&config);

#else    
    setup_ap();
#endif
    setup_coap();
}