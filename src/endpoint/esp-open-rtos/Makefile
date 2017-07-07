# esp-open-rtos specific 

PROGRAM=main

#PROGRAM_INC_DIR += ../../../ext/mbedtlscpp

EXTRA_COMPONENTS = extras/mbedtls extras/dhcpserver

CPPFLAGS += -DESP_OPEN_RTOS

PROGRAM_CPPFLAGS = $(CPPFLAGS) -std=c++11

include $(ESP_OPEN_RTOS)/common.mk