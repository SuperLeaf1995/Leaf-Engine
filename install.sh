#!/bin/sh

make -fMakefile.gcc clean || exit

make -fMakefile.gcc linux-static || exit
sudo make -fMakefile.gcc install-linux-static ||exit
make -fMakefile.gcc linux-shared || exit
sudo make -fMakefile.gcc install-linux-shared || exit
echo "Done"

exit
