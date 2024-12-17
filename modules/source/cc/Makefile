C_SRCS = ast.c ingest.c lexer.c parser.c preprocessor.c
MP_C_FLAGS = -I$(MP_MODULES_INCLUDE) -fPIC
MP_LD_FLAGS = -shared
BUILD_DIR = $(MP_BUILD_ROOT_DIR)/cc
TARGET = $(MP_BUILD_ROOT_DIR)/magpie-cc.so

include $(MP_MODULE_MAKEFILE_COMMON)
