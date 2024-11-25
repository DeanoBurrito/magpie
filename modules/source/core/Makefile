C_SRCS = alloc.c config.c diagnostics.c nanoprintf.c plugin.c source.c
MP_C_FLAGS = -I$(MP_MODULES_INCLUDE) -fPIC
MP_LD_FLAGS = -shared
BUILD_DIR = $(MP_BUILD_ROOT_DIR)/core
TARGET = $(BUILD_DIR)/magpie-core.so

C_FLAGS += $(MP_C_FLAGS)
LD_FLAGS += $(MP_LD_FLAGS)
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SRCS))

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

$(TARGET): $(OBJS)
	$(LD) $(OBJS) $(LD_FLAGS) -o $(TARGET)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(C_FLAGS) -c $< -o $@
