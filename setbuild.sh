#!/bin/sh

BUILD="DEBUG"

if test "$1" == "debug"
then
    BUILD="DEBUG"
elif test "$1" == "release"
then
    BUILD="RELEASE"
elif test "$1" == "debugsilent"
then
    BUILD=DEBUGSILENT
elif test "$1" == "mastering"
then
    BUILD=MASTERING
fi


make -f mk/configure.mk BUILD=$BUILD