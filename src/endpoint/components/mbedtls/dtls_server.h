#pragma once

#include <stdint.h>
#include <stddef.h>

// TODO: These functions need refinement.  For example no error checking
// mechanism is present
typedef void (*dtls_server_request_handler_t)(const uint8_t* buffer, size_t length, void* context);
typedef size_t (*dtls_server_response_handler_t)(uint8_t* buffer, size_t max_length, void* context);


extern dtls_server_request_handler_t    dtls_server_request_handler;
extern dtls_server_response_handler_t   dtls_server_response_handler;

// TODO: stick this out into framework abstraction area
#if defined(ESP_PLATFORM) || defined(ESP_OPEN_RTOS)
#define RTOS_FREERTOS
#endif

#ifdef RTOS_FREERTOS
int dtls_server_handler( void );
#endif