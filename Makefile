CC = gcc
CFLAGS = -Wall -Wextra -ansi -pedantic

OUT_DIR = out
SRC_DIR = src
INC_DIR = src

all: $(OUT_DIR)/leaf.a
	echo Done compiling LEAF library

$(OUT_DIR)/leaf.a: $(OUT_DIR)/mlib.obj $(OUT_DIR)/video.obj $(OUT_DIR)/leaf.obj $(OUT_DIR)/palette.obj $(OUT_DIR)/pcspeak.obj $(OUT_DIR)/bmp.obj $(OUT_DIR)/pcx.obj $(OUT_DIR)/tiff.obj
	ar rcs $@ $^

$(OUT_DIR)/bmp.obj: $(SRC_DIR)/bmp.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $^ -o $@
	
$(OUT_DIR)/mlib.obj: $(SRC_DIR)/mlib.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $^ -o $@

$(OUT_DIR)/palette.obj: $(SRC_DIR)/palette.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $^ -o $@

$(OUT_DIR)/pcspeak.obj: $(SRC_DIR)/pcspeak.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $^ -o $@

$(OUT_DIR)/pcx.obj: $(SRC_DIR)/pcx.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $^ -o $@

$(OUT_DIR)/tiff.obj: $(SRC_DIR)/tiff.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $^ -o $@
	
$(OUT_DIR)/video.obj: $(SRC_DIR)/video.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $^ -o $@
