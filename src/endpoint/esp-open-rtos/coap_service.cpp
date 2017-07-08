#include <stdlib.h>
#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "esp8266.h"

#include "coap_lwip.hpp"

//const uint16_t rsplen = 128;
//static char rsp[rsplen] = "";
const coap_resource_path_t path_well_known_core = {2, {".well-known", "core"}};

coap_resource_t resources[] =
{
    /*
    {COAP_RDY, COAP_METHOD_GET, COAP_TYPE_ACK,
        handle_get_well_known_core, &path_well_known_core,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_APP_LINKFORMAT)},
    {COAP_RDY, COAP_METHOD_GET, COAP_TYPE_ACK,
        handle_get_light, &path_light,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_TXT_PLAIN)},
    {COAP_RDY, COAP_METHOD_PUT, COAP_TYPE_ACK,
        handle_put_light, &path_light,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_NONE)},
    {(coap_state_t)0, (coap_method_t)0, (coap_msgtype_t)0,
        NULL, NULL,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_NONE)} */
};


//yacoap::CoapManager<resources> coapManager;
yacoap::CoapServer<resources> coapServer;

/*
void resource_setup(const coap_resource_t *resources)
{
    coap_make_link_format(resources, rsp, rsplen);
    printf("resources: %s\n", rsp);
} */


void coapTask(void *pvParameters)
{
    for(;;)
    {
        coapServer.handler();
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}



void setup_coap()
{
    //resource_setup(resources);

    // TODO: really want to hang off DTLS task, since it will have a massive
    // stack
    // while we're building things out, run a non-DTLS version 
    xTaskCreate(coapTask, "coap", 1024, NULL, 2, NULL);
}