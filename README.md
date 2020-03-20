# Leaf Engine
A game engine for making DOS games

# Use game engine
For using the game engine, download lengine.h (it's in the repo), download a copy of the .OBJ file (see releases) or make your own (see below). You will need to download TurboC from archive.org and install DOSBox. Once you have TurboC place the TC folder on a folder called "mygame" (You can call it whateever you want), and place LEAF.OBJ into the folder, so it looks like this
mygame
--TC
----TCC.EXE
----(many other files)
--LEAF.OBJ
--LENGINE.H
Create a MAIN.C file in the mygame folder with the following contents:
```c
#include "lengine.h"

int main(void) {
	initMouse();
	setMouseStatus(MOUSE_STATUS_SHOW);
	while(!kbhit()) {}
	setMouseStatus(MOUSE_STATUS_HIDE);
	return 0;
}
```
Open DOSBox in the mygame folder:
```
dosbox ./
```
Then execute the following commands:
```
cd leaf
makegame.bat
```

# Compilation
To compile Leaf Engine, first install DOSBox. Then you need to create a folder, call it whateever you want, but keep the name super short. Once there, download a copy of TurboC available in archive.org Copy the folder TC into your new folder. Then download or clone the repository. Then place Leaf Engine inside a folder called "leaf" (without quotes). Then place that folder into your new folder, The structure of the folder should look like this:

newfolder
--TC
----TCC.exe
----(many other files)
--leaf
----src
----out
----docs
----MAKE.BAT
----LICENSE
----README.md

If this is correct execute the next command in the folder you made:
```
dosbox ./
```
When DOSBox opens, and C: is mounted, input the following:
```
cd leaf
make.bat
```
Wait until the compilation ends. Once it finishes, a file called LEAF.OBJ will spawn, and that's it, you now have a game engine compiled.
