# Leaf Engine
A powerful game engine for making DOS and Linux games.

# Compiling library
Compiling the game engine is relatively easy on most UNIX and DOS systems.

## Linux
Get ``git`` via your package manager (yum,apt-get,apt,pacman,etc). Then execute:
```
git clone https://github.com/Superleaf1995/leaf-engine
cd leaf-engine
```
After that, execute distro-specific commands (see below).

### Arch
Run the following:
```
pacman -S gcc make build-essential libx11
make
make install
```

### Debian
Run the following:
```
sudo apt-get install gcc build-essential libx11
make
sudo make install
```

## Fedora
Run the following:
```
yum install gcc build-essential libx11
make
sudo make install
```

## Ubuntu
Run the following:
```
sudo apt install gcc build-essential libx11
make
sudo make install
```

## DOS
Get a copy of the game engine, either via ``git clone`` or via direct download. Then execute:
```
cd leaf-engine
```
After that, execute compiler-specific commands (see below)

### Borland TurboC++
Run the following:
```
tcc -w -mh -c src\leaf.c -o out\leaf.o
```

### DJGPP
Run the following:
```
make
make install
```

### TurboC
Run the following:
```
tcc -w -mh -c src\leaf.c -o out\leaf.o
```

## BSD
Get ``git`` via your package manager (pkg,etc). Then execute:
```
git clone https://github.com/Superleaf1995/leaf-engine
cd leaf-engine
```
After that, execute distro-specific commands (see below).

### FreeBSD
Run the following:
```
git clone https://github.com/Superleaf1995/leaf-engine
cd leaf-engine
pkg install gcc make libx11
make
sudo make install
```
