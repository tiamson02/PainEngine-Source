#!/bin/sh

make clean
rm -rf example/umd_psp_root
rm -rf example/umd_psp_scripts

make

cd example
./make_psp_zipfile.sh
cd ..