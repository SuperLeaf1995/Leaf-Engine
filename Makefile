# Makefile for TurboC Leaf-Engine builds

# Change accordingly
CC = c:\tc\tcc
CFLAGS = -O -r -Z -mh -j5 -g8 -Isrc -Iinclude
CLIB = c:\tc\tlib

all: out\dos\leaf.lib
	@echo "--- BUILDING DONE ---"

install: out\dos\leaf.lib
	@echo "--- INSTALLING ---"
	copy out\dos\leaf.lib c:\tc\lib
	copy include c:\tc\include
	$(CLIB) c:\tc\lib\cs.lib +out\dos\leaf.lib
	@echo "--- INSTALLING DONE ---"
	
uninstall: out\dos\leaf.lib
	@echo "--- UNINSTALLING ---"
	$(CLIB) c:\tc\lib\cs.lib -out\dos\leaf.lib
	$(CLIB) c:\tc\lib\cs.lib -out\dos\fdata.obj-out\dos\fbmp.obj-out\dos\fpcx.obj-out\dos\graphic.obj
	del c:\tc\include\leaf.h
	del c:\tc\lib\leaf.lib
	@echo "--- UNINSTALLING DONE ---"

clean:
	del out\dos\*.*

out\dos\leaf.lib: out\dos\fdata.obj out\dos\fbmp.obj out\dos\fpcx.obj out\dos\graphic.obj
	@echo "--- BUILDING $< ---"
	$(CLIB) /C $< +out\dos\fdata.obj+out\dos\fbmp.obj+out\dos\fpcx.obj+out\dos\graphic.obj

out\dos\fdata.obj: src\fdata.c src\fdata.h
	@echo "--- BUILDING $< ---"
	$(CC) -c $(CFLAGS) -o$< src\fdata.c

out\dos\fbmp.obj: src\fbmp.c src\fbmp.h
	@echo "--- BUILDING $< ---"
	$(CC) -c $(CFLAGS) -o$< src\fbmp.c

out\dos\fpcx.obj: src\fpcx.c src\fpcx.h
	@echo "--- BUILDING $< ---"
	$(CC) -c $(CFLAGS) -o$< src\fpcx.c

out\dos\graphic.obj: src\graphic.c src\graphic.h
	@echo "--- BUILDING $< ---"
	$(CC) -c $(CFLAGS) -o$< src\graphic.c

out\dos\mouse.obj: src\mouse.c src\mouse.h
	@echo "--- BUILDING $< ---"
	$(CC) -c $(CFLAGS) -o$< src\mouse.c
