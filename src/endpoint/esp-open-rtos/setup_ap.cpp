extern "C" {

#include <lwip/ip.h>
#include <lwip/netif.h>
#include <lwip/api.h>

#include "dhcpserver.h"

#include <espressif/esp_common.h>

#include <string.h>

}

const char AP_SSID[] = "esp-open-rtos AP";


// Lifted from https://github.com/SuperHouse/esp-open-rtos/blob/master/examples/access_point/access_point.c
void setup_ap()
{
    /*
    ip4_addr_t start_addr;// = { 192, 168, 4, 2 };

    IP4_ADDR(&start_addr, 192, 168, 4, 2);

    dhcpserver_start(&start_addr, 3); */

    sdk_wifi_set_opmode(SOFTAP_MODE);
    struct ip_info ap_ip;
    IP4_ADDR(&ap_ip.ip, 172, 16, 0, 1);
    IP4_ADDR(&ap_ip.gw, 0, 0, 0, 0);
    IP4_ADDR(&ap_ip.netmask, 255, 255, 0, 0);
    sdk_wifi_set_ip_info(1, &ap_ip);

    struct sdk_softap_config ap_config;
    
    strcpy((char*) ap_config.ssid, AP_SSID);
    ap_config.ssid_hidden = 0;
    ap_config.channel = 3;
    ap_config.ssid_len = strlen(AP_SSID);
    ap_config.authmode = AUTH_OPEN;
    memset(ap_config.password, 0, sizeof(ap_config.password));
    ap_config.max_connection = 3;
    ap_config.beacon_interval = 100;

    sdk_wifi_softap_set_config(&ap_config);

    ip_addr_t first_client_ip;
    IP4_ADDR(&first_client_ip, 172, 16, 0, 2);
    dhcpserver_start(&first_client_ip, 4);
}