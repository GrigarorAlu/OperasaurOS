BOOT_SRC_DIR = $(SRC_DIR)/boot
BOOT_OBJ_DIR = $(OBJ_DIR)/boot

BOOT_SRC = $(BOOT_SRC_DIR)/$(TARGET_PLATFORM)/boot.s
BOOT_OBJ = $(patsubst $(BOOT_SRC_DIR)/%,$(BOOT_OBJ_DIR)/%.o,$(BOOT_SRC))
BOOT_TARGET = $(BOOT_OBJ_DIR)/boot.bin

$(BOOT_TARGET): $(BOOT_OBJ)
	mkdir -p $(@D)
	$(LD) -T $(BOOT_SRC_DIR)/$(TARGET_PLATFORM)/linker.ld --oformat binary $^ -o $@

$(BOOT_OBJ_DIR)/%.o: $(BOOT_SRC_DIR)/%
	mkdir -p $(@D)
	$(AS) $^ -o $@

