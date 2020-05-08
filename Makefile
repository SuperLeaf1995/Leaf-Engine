# Makefile for GCC Leaf-Engine builds

OUT_DIR = out
INC_DIR = src
SRC_DIR = src

AR = ar
CC = gcc
CFLAGS = -Wall -Wextra -ansi -pedantic -I$(INC_DIR)
CROSS_PATH = ~/opt

GBA_CC = $(CROSS_PATH)/gcc-arm-none-eabi/bin/arm-none-eabi-gcc
GBA_AR = $(CROSS_PATH)/gcc-arm-none-eabi/bin/arm-none-eabi-ar

# All-target
all: gba linux-shared linux-static

linux-shared: $(OUT_DIR)/linux $(OUT_DIR)/linux/libleaf.so
linux-static: $(OUT_DIR)/linux $(OUT_DIR)/linux/libleaf.a
gba: $(OUT_DIR)/gba $(OUT_DIR)/gba/libleaf.a

install:
	cp $(OUT_DIR)/libleaf.so /usr/lib
	cp $(SRC_DIR)/leaf.h /usr/include

clean:
	rm out/linux/*.*

# Directories
$(OUT_DIR)/linux: $(OUT_DIR)
	mkdir -p $@
	
$(OUT_DIR)/gba: $(OUT_DIR)
	mkdir -p $@

# Linux (Static)
$(OUT_DIR)/linux/libleaf.a: $(OUT_DIR)/linux/leaf.obj
	$(AR) rcs $@ $^

$(OUT_DIR)/linux/%.obj: $(SRC_DIR)/%.c $(INC_DIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@
	
# Linux (Shared)
$(OUT_DIR)/linux/libleaf.so: $(OUT_DIR)/linux/leaf.o
	$(CC) -shared $^ -o $@

$(OUT_DIR)/linux/%.o: $(SRC_DIR)/%.c $(INC_DIR)/%.h
	$(CC) $(CFLAGS) -fpic -c $< -o $@

# GBA
$(OUT_DIR)/gba/libleaf.a: $(OUT_DIR)/gba/leaf.obj
	$(GBA_AR) rcs $@ $^

$(OUT_DIR)/gba/%.obj: $(SRC_DIR)/%.c $(INC_DIR)/%.h
	$(GBA_CC) -D__GBA__ $(CFLAGS) -c $< -o $@
