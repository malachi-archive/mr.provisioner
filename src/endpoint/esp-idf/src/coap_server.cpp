#ifdef MRPROVISIONER_FEATURE_DTLS_ENABLE

// TODO: Move this out into components/freertos

extern "C" {

#include "dtls_server.h"
#include <stdio.h>

dtls_server_request_handler_t    dtls_server_request_handler;
dtls_server_response_handler_t   dtls_server_response_handler;
int dtls_server_handler( void );

}

void coaps_request_handler(const uint8_t* buffer, size_t length, void* context)
{
    printf("COAP: Got a buffer %d long\n", length);
}



void coap_server_task(void* pvParameters)
{
    dtls_server_request_handler = coaps_request_handler;
    
    for(;;)
    {
        dtls_server_handler();
    }
}

#else
void coap_server_task(void* pvParameters)
{
    for(;;)
    {

    }
}

#endif

