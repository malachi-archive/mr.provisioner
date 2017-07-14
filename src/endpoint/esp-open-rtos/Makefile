# esp-open-rtos specific 

PROGRAM=main

EXT_DIR = ../../../ext
#PROGRAM_INC_DIR += ../../../ext/mbedtlscpp
PROGRAM_INC_DIR += $(EXT_DIR)/coapcpp
PROGRAM_INC_DIR += $(EXT_DIR)/lwipcpp

#PROGRAM_INC_DIR += ../components/mbedtls
#PROGRAM_SRC_DIR += ../components/mbedtls

EXTRA_COMPONENTS = extras/mbedtls extras/dhcpserver $(EXT_DIR)/yacoap $(EXT_DIR)/util.embedded ../components/mbedtls

CPPFLAGS += -DESP_OPEN_RTOS
# compiles but not yet tested.  Test out with Android client more before
# enabling this OR get our hands on a good COAPS tester (Copper seems to
# only do COAP not COAPS)
#CPPFLAGS += -DMRPROVISIONER_FEATURE_DTLS_ENABLE

PROGRAM_CPPFLAGS = $(CPPFLAGS) -std=c++11

include $(ESP_OPEN_RTOS)/common.mk