C_SRCS = $(MP_HOST_SRCS) main.c
MP_C_FLAGS = -I$(MP_MODULES_INCLUDE)
MP_LD_FLAGS = -Wl,-rpath,. -L$(MP_BUILD_ROOT_DIR) \
	-l:magpie-core.so -l:magpie-cc.so
BUILD_DIR = $(MP_BUILD_ROOT_DIR)/magpiler
TARGET = $(MP_BUILD_ROOT_DIR)/magpiler.elf

include $(MP_MODULE_MAKEFILE_COMMON)
