# necessary for esp-open-rtos since it despises symlinking

PROGRAM_INC_DIR += $(mbedtls_dtls_ROOT)

mbedtls_dtls_INC_DIR += $(mbedtls_dtls_ROOT)
mbedtls_dtls_SRC_DIR += $(mbedtls_dtls_ROOT)

$(eval $(call component_compile_rules,mbedtls_dtls))