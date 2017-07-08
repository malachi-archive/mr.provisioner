INC_DIRS += $(coap_ROOT)YaCoAP

coap_INC_DIR += $(coap_ROOT)include
coap_SRC_DIR += $(coap_ROOT)YaCoAP

$(eval $(call component_compile_rules,coap))