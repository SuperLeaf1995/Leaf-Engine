#makefile for making the samples

all: out out/gba-test.gba

out:
	mkdir -p out

clean:
	rm -f out/*.gba
	rm -f out/*.elf
	rm -f out/*.obj
	
CFLAGS =	-Werror						\
			-Wall						\
			-Wextra						\
			-Waddress					\
			-Wdate-time					\
			-Wformat-y2k				\
			-Wdouble-promotion			\
			-Wold-style-definition		\
			-Wmissing-prototypes		\
			-Wstrict-prototypes			\
			-fsanitize=address			\
			-fsanitize=undefined		\
			-fsanitize=leak				\
			-fstack-protector-all		\
			-fno-omit-frame-pointer		\
			-fstrict-aliasing			\
			-std=c89					\
			-ansi						\
			-pedantic					\
			-O3							\
			-g

# gba-test
out/gba-test.elf: out/gba-test-entry.obj out/gba-test.obj
	~/opt/gcc-arm-none-eabi/bin/arm-none-eabi-ld $^ -o $@
out/gba-test-entry.obj: gba-test.s
	~/opt/gcc-arm-none-eabi/bin/arm-none-eabi-as $< -o $@
out/gba-test.obj: gba-test.c
	~/opt/gcc-arm-none-eabi/bin/arm-none-eabi-gcc -D__GBA__ -I../src -L../out/gba -lleaf -Wall -Wextra -ansi -pedantic -c $^ -o $@
out/gba-test.gba: out/gba-test.elf
	~/opt/gcc-arm-none-eabi/bin/arm-none-eabi-objcopy -O binary $< $@

# test
out/test.o: test.c
	gcc $(CFLAGS) -lopenal -lX11 $^ /usr/lib/libleaf.a -o $@
