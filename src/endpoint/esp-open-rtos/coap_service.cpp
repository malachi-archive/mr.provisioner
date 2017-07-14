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

const coap_resource_path_t path_ssid_name = {2, {"ssid", "name"}};
const coap_resource_path_t path_ssid_pass = {2, {"ssid", "pass"}};                                       

static int handle_put_ssid_name(const coap_resource_t *resource,
                            const coap_packet_t *inpkt,
                            coap_packet_t *pkt);

struct sdk_station_config config;

coap_resource_t resources[] =
{
    {COAP_RDY, COAP_METHOD_GET, COAP_TYPE_ACK,
        _handle_get_well_known_core, &path_well_known_core,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_APP_LINKFORMAT)},
    {COAP_RDY, COAP_METHOD_PUT, COAP_TYPE_ACK,
        handle_put_ssid_name, &path_ssid_name,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_NONE)}, // might want to set this to TXT_PLAIN, not sure if this refers to incoming or outgoing
    
    /*
    {COAP_RDY, COAP_METHOD_GET, COAP_TYPE_ACK,
        handle_get_light, &path_light,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_TXT_PLAIN)},
    {COAP_RDY, COAP_METHOD_PUT, COAP_TYPE_ACK,
        handle_put_light, &path_light,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_NONE)}, */
    COAP_RESOURCE_NULL
};

static int handle_put_ssid_name(const coap_resource_t *resource,
                            const coap_packet_t *inpkt,
                            coap_packet_t *pkt)
{
    static uint8_t dummy = 0;

    printf("handle_put_ssid_name\n");
    if (inpkt->payload.len == 0) {
        return coap_make_response(inpkt->hdr.id, &inpkt->tok,
                                  COAP_TYPE_ACK, COAP_RSPCODE_BAD_REQUEST,
                                  NULL, NULL, 0,
                                  pkt);
    }
    /*
    if (inpkt->payload.p[0] == '1') {
        light = '1';
        printf("Light ON\n");
    }
    else {
        light = '0';
        printf("Light OFF\n");
    }*/
    printf("ssid name = %s\n", inpkt->payload.p);

    /* TODO: we'll do this once we get our hands on PASS
       TODO: eventually make a POST so that we get NAME and PASS at the same time
    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&config);*/
    // TODO: revisit the idea of a WiFi service/abstractor but since a "true" service
    // is so complicated, treat it more as a low level framework abstraction 
    // (consider actually placing it IN frab)
    // see https://github.com/ourairquality/esp-open-rtos/blob/ourairquality/examples/mqtt_client/mqtt_client.c
    // for bits and pieces needed to hop onto wifi.  Follow their lead and make a separate RTOS task
    // NOTE: theirs uses printf with only 256 stack space.  seems dangerous!

    return coap_make_response(inpkt->hdr.id, &inpkt->tok,
                              COAP_TYPE_ACK, COAP_RSPCODE_CHANGED,
                              resource->content_type,
                              // not sure if we can return a null here
                              &dummy, 1,
                              pkt);
}

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