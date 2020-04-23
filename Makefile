# Builds a Leaf-Engine shared library

OUT_DIR = out
INC_DIR = src
SRC_DIR = src

LD = ld
CC = gcc
CFLAGS = -fpic -Wall -Wextra -ansi -pedantic -lX11 -I$(INC_DIR)

all: $(OUT_DIR)/libleaf.so

install:
	cp $(OUT_DIR)/libleaf.so /usr/lib
	cp $(SRC_DIR)/leaf.h /usr/include

clean:
	rm out/*.*

$(OUT_DIR)/libleaf.so: $(OUT_DIR)/leaf.o
	$(LD) -shared $^ -o $@
	
$(OUT_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/%.h
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@
