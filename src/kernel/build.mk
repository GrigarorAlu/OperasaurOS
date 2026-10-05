KERNEL_SRC_DIR = $(SRC_DIR)/kernel
KERNEL_OBJ_DIR = $(OBJ_DIR)/kernel

KERNEL_PLATFORM_DIR = $(KERNEL_SRC_DIR)/platform/$(TARGET_PLATFORM)

KERNEL_SRC = $(wildcard $(KERNEL_SRC_DIR)/*.c \
	$(KERNEL_PLATFORM_DIR)/*.c $(KERNEL_PLATFORM_DIR)/*.s \
	$(KERNEL_PLATFORM_DIR)/*/*.c $(KERNEL_PLATFORM_DIR)/*/*.s \
	$(KERNEL_SRC_DIR)/lib/*.c $(KERNEL_SRC_DIR)/lib/*.s)
KERNEL_OBJ = $(patsubst $(KERNEL_SRC_DIR)/%,$(KERNEL_OBJ_DIR)/%.o,$(KERNEL_SRC))
KERNEL_TARGET = $(KERNEL_OBJ_DIR)/kernel.bin

$(KERNEL_TARGET): $(KERNEL_OBJ)
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) -T $(KERNEL_SRC_DIR)/linker.ld $^ -o $@ -lgcc

$(KERNEL_OBJ_DIR)/%.o: $(KERNEL_SRC_DIR)/%
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -I $(SRC_DIR)/kernel/lib -c $^ -o $@

