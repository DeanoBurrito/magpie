ENABLED_MODULES = core cc
ENABLED_TOOLS = magpiler

TOP_TARGETS = all clean
export MP_MODULES_INCLUDE = $(abspath modules/include)
export MP_TOOLS_INCLUDE = $(abspath tools/include)
export MP_BUILD_ROOT_DIR = $(abspath .build)
export MP_MODULE_MAKEFILE_COMMON = $(abspath common.mk)
export MP_HOST_SRCS = $(abspath hosts/linux.c)
export MP_GLOBAL_C_FLAGS = -g

.PHONY: $(ENABLED_MODULES) $(ENABLED_TOOLS) $(TOP_TARGETS)
$(TOP_TARGETS): $(ENABLED_MODULES) $(ENABLED_TOOLS)

$(ENABLED_MODULES):
	$(MAKE) -C modules/source/$@ $(MAKECMDGOALS)

$(ENABLED_TOOLS):
	$(MAKE) -C tools/source/$@ $(MAKECMDGOALS)

.PHONY: debug
debug:
	cd .build; gdb magpiler.elf
