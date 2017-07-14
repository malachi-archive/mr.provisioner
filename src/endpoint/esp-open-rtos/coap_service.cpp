#define DEBUG_ENABLE1

#include <stdlib.h>
#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "esp8266.h"

#include "coap_lwip.hpp"

#ifdef MRPROVISIONER_FEATURE_DTLS_ENABLE
extern "C" {

#include "dtls_server.h"

}
#endif

using namespace FactUtilEmbedded::std;

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
static int handle_put_ssid_pass(const coap_resource_t *resource,
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
    {COAP_RDY, COAP_METHOD_PUT, COAP_TYPE_ACK,
        handle_put_ssid_pass, &path_ssid_pass,
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
    // FIX: Kludgey and not sure how reliable it is to write back into this buffer
    auto len = inpkt->payload.len;
    //inpkt->payload.p[len] = 0;
    memcpy(config.ssid, inpkt->payload.p, len);
    config.ssid[len] = 0;
    printf("ssid name = %s\n", config.ssid);

    //strcpy((char*)config.ssid, (char*)inpkt->payload.p); 

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


static int handle_put_ssid_pass(const coap_resource_t *resource,
                            const coap_packet_t *inpkt,
                            coap_packet_t *pkt)
{
    static uint8_t dummy = 0;

    printf("handle_put_ssid_pass\n");
    if (inpkt->payload.len == 0) {
        return coap_make_response(inpkt->hdr.id, &inpkt->tok,
                                  COAP_TYPE_ACK, COAP_RSPCODE_BAD_REQUEST,
                                  NULL, NULL, 0,
                                  pkt);
    }

    // FIX: Kludgey and not sure how reliable it is to write back into this buffer
    auto len = inpkt->payload.len;
    //inpkt->payload.p[len] = 0;
    memcpy(config.password, inpkt->payload.p, len);
    config.password[len] = 0;
    printf("ssid pass = %s\n", config.password);

    //strcpy((char*)config.password, (char*)inpkt->payload.p); 

    /*
        sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&config);
    */

    return coap_make_response(inpkt->hdr.id, &inpkt->tok,
                              COAP_TYPE_ACK, COAP_RSPCODE_CHANGED,
                              resource->content_type,
                              // not sure if we can return a null here
                              &dummy, 1,
                              pkt);
}

#ifdef MRPROVISIONER_FEATURE_DTLS_ENABLE
// presently CoapServer is hardwired to LWIP
// not a terrible thing, but that precludes drop in compatibility for mbedtls
yacoap::CoapManager<resources> coapManager;
// FIX: put this into context eventually.  For now, this global is OK in context
// of normal embedded behavior - just totally non reentrant friendly
yacoap::CoapPacket coapResponse;

static void coaps_request_handler(const uint8_t* buffer, size_t len, void* context)
{
    yacoap::CoapRequest request(buffer, len);

    // TODO: consolidate this in a more open ended CoapServer
    if (request.getResult() > COAP_ERR)
        clog << "Bad packet rc=" << request.getResult() << endl;
    else
    {
        coapManager.handleRequest(request, coapResponse);
    }
}

static size_t coaps_response_handler(uint8_t* buffer, size_t max_len, void* context)
{
    // TODO: assert we don't exceed max_len

    int rc;
    size_t buflen;

    if ((rc = coapResponse.build(buffer, &buflen)) > COAP_ERR)
    {
        clog << "coap_build failed rc=" << rc << endl;
        return -1;
    }
    else
    {
#ifdef YACOAP_DEBUG
        coapResponse.dump();
#endif
        return buflen;
    }
}


dtls_server_request_handler_t    dtls_server_request_handler = coaps_request_handler;
dtls_server_response_handler_t   dtls_server_response_handler = coaps_response_handler;

static int _handle_get_well_known_core(const coap_resource_t *resource,
                                          const coap_packet_t *inpkt,
                                          coap_packet_t *pkt)
{
    return coapManager.handle_get_well_known_core(resource, inpkt, pkt);
}


void coapTask(void *pvParameters)
{
    for(;;)
    {
        //coapServer.handler();
    }
}
#else

yacoap::CoapServer<resources> coapServer;

static int _handle_get_well_known_core(const coap_resource_t *resource,
                                          const coap_packet_t *inpkt,
                                          coap_packet_t *pkt)
{
    return coapServer.handle_get_well_known_core(resource, inpkt, pkt);
}



void coapTask(void *pvParameters)
{
    for(;;)
    {
        coapServer.handler();
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