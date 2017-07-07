INC_DIRS += $(libcoap_ROOT)libcoap/include/coap

libcoap_INC_DIR += $(PROGRAM_ROOT)
libcoap_SRC_DIR += $(libcoap_ROOT)libcoap/src

# OK don't try to pull in EVERYTHING just yet!!
#sming_SRC_DIR += $(sming_ROOT)Sming/Sming/SmingCore

$(eval $(call component_compile_rules,libcoap))

#$(info INC_DIRS = ${INC_DIRS})
#$(info Component root = ${sming_ROOT})
#$(info Component inc_dir = ${sming_INC_DIR})