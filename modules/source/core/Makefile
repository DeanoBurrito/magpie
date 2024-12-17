C_SRCS = alloc.c config.c diagnostics.c nanoprintf.c plugin.c source.c string.c
MP_C_FLAGS = -I$(MP_MODULES_INCLUDE) -fPIC
MP_LD_FLAGS = -shared
BUILD_DIR = $(MP_BUILD_ROOT_DIR)/core
TARGET = $(MP_BUILD_ROOT_DIR)/magpie-core.so

include $(MP_MODULE_MAKEFILE_COMMON)
