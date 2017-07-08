#define DEBUG_ENABLE1

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

//extern coap_resource_t resources[];
static int _handle_get_well_known_core(const coap_resource_t *resource,
                                          const coap_packet_t *inpkt,
                                          coap_packet_t *pkt);

coap_resource_t resources[] =
{
    {COAP_RDY, COAP_METHOD_GET, COAP_TYPE_ACK,
        _handle_get_well_known_core, &path_well_known_core,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_APP_LINKFORMAT)},
    /*
    {COAP_RDY, COAP_METHOD_GET, COAP_TYPE_ACK,
        handle_get_light, &path_light,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_TXT_PLAIN)},
    {COAP_RDY, COAP_METHOD_PUT, COAP_TYPE_ACK,
        handle_put_light, &path_light,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_NONE)}, */
    COAP_RESOURCE_NULL
};


//yacoap::CoapManager<resources> coapManager;

yacoap::CoapServer<resources> coapServer;

static int _handle_get_well_known_core(const coap_resource_t *resource,
                                          const coap_packet_t *inpkt,
                                          coap_packet_t *pkt)
{
    return coapServer.handle_get_well_known_core(resource, inpkt, pkt);
}

#ifdef DEBUG_ENABLE1



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
        printf("\nRecycling");
    }
}
#endif


void setup_coap()
{
    //resource_setup(resources);

    // TODO: really want to hang off DTLS task, since it will have a massive
    // stack
    // while we're building things out, run a non-DTLS version 
    xTaskCreate(coapTask, "coap", 1024, NULL, 2, NULL);
}