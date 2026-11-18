#!/bin/sh

if test -e mk/conf.mk
then
    echo build mode is already set
else
    echo build mode not set... choosing PSP debug as the default
    make -f mk/configure.mk BUILD=DEBUG OS=PSPDEVKIT COMPILER=GCC3
fi

# change the build configuration to MASTERING and rebuild the project
cp mk/conf.mk mk/conf.mk.bak
make -f mk/configure.mk BUILD=MASTERING
make clean
make

#rebuild the data packs and build the ISO
cd example
./make_psp_umd_layout.sh
cd ..

#restore the previous build configuration
cp mk/conf.mk.bak mk/conf.mk
rm mk/conf.mk.bak

#zip up the resulting ISO and post it on the server
ISO_NAME="painkiller-psp-master-`date +%b-%d-%Y`.zip"
zip -9 $ISO_NAME example/umd_layout/umd_images/*