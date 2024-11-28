override C_FLAGS += $(MP_C_FLAGS) $(MP_GLOBAL_C_FLAGS)
override LD_FLAGS += $(MP_LD_FLAGS)
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SRCS))

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	-rm -r $(BUILD_DIR)
	-rm -r $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LD_FLAGS) -o $(TARGET)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(C_FLAGS) -c $< -o $@
