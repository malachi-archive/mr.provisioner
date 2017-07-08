INC_DIRS += $(util_embedded_ROOT)util.embedded/src

util_embedded_SRC_DIR += $(util_embedded_ROOT)util.embedded/src
util_embedded_SRC_DIR += $(util_embedded_ROOT)util.embedded/src/fact
util_embedded_SRC_DIR += $(util_embedded_ROOT)util.embedded/src/fact/streams
util_embedded_SRC_DIR += $(util_embedded_ROOT)util.embedded/src/fact/hardware
#util_embedded_SRC_DIR += $(util_embedded_ROOT)util.embedded/src/driver/atmel/sam
# OK don't try to pull in EVERYTHING just yet!!
#sming_SRC_DIR += $(sming_ROOT)Sming/Sming/SmingCore
util_embedded_CPPFLAGS += -DESP_OPEN_RTOS -DFEATURE_IOSTREAM_SHIM -std=c++11 -DNODUINO

$(eval $(call component_compile_rules,util_embedded))