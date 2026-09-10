#!/bin/bash
#
#	Just a simple bash script to build executables for this lab
#
#	I don't try to go fancy.  I just hard-code my paths and commands.

SRC_DIR="ImageLibrary/src"
HDR_DIR="ImageLibrary/include"
APP_DIR="ImageLibrary/applications"
EXE_DIR="ImageLibrary/executables"

#	flipV
gcc -Wall -std=c11 -I $HDR_DIR $SRC_DIR/*.c $APP_DIR/flipV.c -o $EXE_DIR/flipV

#	flipH
gcc -Wall -std=c11 -I $HDR_DIR $SRC_DIR/*.c $APP_DIR/flipH.c -o $EXE_DIR/flipH

#	channel
gcc -Wall -std=c11 -I $HDR_DIR $SRC_DIR/*.c $APP_DIR/channel.c -o $EXE_DIR/channel

#	gray
gcc -Wall -std=c11 -I $HDR_DIR $SRC_DIR/*.c $APP_DIR/gray.c -o $EXE_DIR/gray

echo "done."