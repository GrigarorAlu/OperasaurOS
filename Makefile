TOOLCHAIN_DIR ?= ~/toolchain/bin
TARGET_PLATFORM ?= x86

AS = $(TOOLCHAIN_DIR)/i686-elf-as
CC = $(TOOLCHAIN_DIR)/i686-elf-gcc
LD = $(TOOLCHAIN_DIR)/i686-elf-ld

CFLAGS = -std=gnu99 -ffreestanding -Wall -Wextra -Wno-pointer-sign -Wno-unused-parameter -nostdlib -fstack-protector-all -O2
LDFLAGS = -Wl,--oformat,binary -ffreestanding -nostdlib

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

TARGET = $(BIN_DIR)/os.bin

.PHONY: build run

include src/boot/build.mk
include src/kernel/build.mk

run: build
	#bochs -q -f bochsrc
	qemu-system-i386 -drive format=raw,file=$(TARGET)

build: $(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

$(TARGET): $(BOOT_TARGET) $(KERNEL_TARGET)
	mkdir -p $(@D) 
	cat $^ > $@

