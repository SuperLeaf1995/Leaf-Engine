CC = gcc
CFLAGS = -Wall -Wextra -ansi -pedantic

OUT_DIR = out
SRC_DIR = src
INC_DIR = src

all: $(OUT_DIR)/leaf.obj $(OUT_DIR)/bmp.obj $(OUT_DIR)/pcx.obj $(OUT_DIR)/tiff.obj
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $^ -o $(OUT_DIR)/leaf.o

$(OUT_DIR)/bmp.obj: $(SRC_DIR)/bmp.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $^ -o $@

$(OUT_DIR)/leaf.obj: $(SRC_DIR)/leaf.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $^ -o $@

$(OUT_DIR)/pcx.obj: $(SRC_DIR)/pcx.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $^ -o $@

$(OUT_DIR)/tiff.obj: $(SRC_DIR)/tiff.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $^ -o $@
