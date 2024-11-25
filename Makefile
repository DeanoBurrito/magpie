ENABLED_MODULES = core
ENABLED_TOOLS = 

TOP_TARGETS = all clean
export MP_MODULES_INCLUDE = $(abspath modules/include)
export MP_TOOLS_INCLUDE = $(abspath tools/include)
export MP_BUILD_ROOT_DIR = $(abspath .build)

.PHONY: $(ENABLED_MODULES) $(ENABLED_TOOLS) $(TOP_TARGETS)
$(TOP_TARGETS): $(ENABLED_MODULES) $(ENABLED_TOOLS)

$(ENABLED_MODULES):
	$(MAKE) -C modules/source/$@ $(MAKECMDGOALS)

$(ENABLED_TOOLS):
	$(MAKE) -C tools/source/$@ $(MAKECMDGOALS)
