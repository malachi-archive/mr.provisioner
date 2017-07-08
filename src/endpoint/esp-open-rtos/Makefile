# esp-open-rtos specific 

PROGRAM=main

EXT_DIR = ../../../ext
#PROGRAM_INC_DIR += ../../../ext/mbedtlscpp
PROGRAM_INC_DIR += $(EXT_DIR)/coapcpp
PROGRAM_INC_DIR += $(EXT_DIR)/lwipcpp

EXTRA_COMPONENTS = extras/mbedtls extras/dhcpserver $(EXT_DIR)/yacoap $(EXT_DIR)/util.embedded

CPPFLAGS += -DESP_OPEN_RTOS

PROGRAM_CPPFLAGS = $(CPPFLAGS) -std=c++11

include $(ESP_OPEN_RTOS)/common.mk