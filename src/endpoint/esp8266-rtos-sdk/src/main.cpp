extern "C" {
#include "esp_common.h"
#include "dtls_server.h"
}
#include "config.h"

void conn_ap_init(void)
{
    wifi_set_opmode(STATIONAP_MODE);
    struct station_config config;
    memset(&config, 0, sizeof(config));  //set value of config from address of &config to width of size to be value '0'
    sprintf((char*)config.ssid, WIFI_AP_SSID);
    sprintf((char*)config.password, WIFI_AP_PASS);
    wifi_station_set_config(&config);
    //wifi_set_event_handler_cb(wifi_handle_event_cb);
    wifi_station_connect();
}

void dtls_wrapper(void*)
{
    for(;;)
    {
        dtls_server_handler();
    }
}

void my_loop(void*)
{
    
}

extern "C" void user_init(void)
{
    printf("SDK version:%s\n", system_get_sdk_version());

    xTaskCreate(my_loop, (const signed char*)"my_loop", 512, NULL, 4, NULL);
    xTaskCreate(dtls_wrapper, (const signed char*)"DTLS", 2048, NULL, 4, NULL);
    
}