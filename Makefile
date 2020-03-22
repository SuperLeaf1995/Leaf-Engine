CC = gcc
CFLAGS = -Wall -Wextra -ansi -pedantic

OUT_DIR = out
SRC_DIR = src
INC_DIR = src

all: $(OUT_DIR)/libleaf.a

$(OUT_DIR)/libleaf.a: $(OUT_DIR)/mlib.o $(OUT_DIR)/video.o $(OUT_DIR)/palette.o $(OUT_DIR)/pcspeak.o $(OUT_DIR)/bmp.o $(OUT_DIR)/pcx.o $(OUT_DIR)/tiff.o
	ar rcs $@ $^

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $^ -o $@
	
install: /usr/include/leaf /usr/lib $(OUT_DIR)/libleaf.a
	cp $(SRC_DIR)/*.h /usr/include/leaf
	cp $(OUT_DIR)/*.a /usr/lib
	
/usr/include/leaf:
	mkdir /usr/include/leaf
	
unistall:
	rm /usr/include/leaf/*.h
	rm /usr/lib/libleaf.a

clean:
	rm out/*.*
