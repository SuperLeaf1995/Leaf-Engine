#makefile for making the samples

all: out/test.o out/poly.o out/3Dwall.o

out:
	mkdir -p out

clean:
	rm -f out/*.gba
	rm -f out/*.elf
	rm -f out/*.obj
	rm -f out/*.o
	
CFLAGS	=	-Wall						\
			-Wextra						\
			-O3							\
			-g							\
			-lopenal					\
			-lX11						\
			-lm							\
			
out/%.o: %.c /usr/lib/libleaf.a
	gcc $(CFLAGS) $^ -o $@
