# Leaf Engine
A powerful game engine for making DOS and Linux games.

# Installing the game engine (DOS)
Download DJGPP and put it in C: drive, follow instructions made on the delorie website. Now download Leaf-Engine repository and then run:
```
make
```
libleaf.a will be placed in the out directory, put it in the same directory as your game's root directory along with the header files in the src directory.

# Installing the game engine (Linux) (WIP)
Run:
```
sudo apt-get install build-essential gcc git
git clone https://github.com/Leaf-Engine/Leaf-Engine.git
```
Open a terminal window in the Leaf-Engine directory, and then enter:
```
make
sudo make install
```
After it ends, you now have Leaf-Engine in your Linux.
