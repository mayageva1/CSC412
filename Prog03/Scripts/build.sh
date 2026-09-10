#!/bin/bash
#
#	Just a simple bash script to build executables for this lab
#
#	I don't try to go fancy.  I just hard-code my paths and commands.

SRC_DIR="ImageLibrary/src"
HDR_DIR="ImageLibrary/include"
APP_DIR="ImageLibrary/applications"
EXE_DIR="ImageLibrary/executables"
MAINAPP_DIR="Applications"

#	flipV
gcc -Wall -std=c11 -I $HDR_DIR $SRC_DIR/*.c $APP_DIR/flipV.c -o $EXE_DIR/flipV

#	flipH
gcc -Wall -std=c11 -I $HDR_DIR $SRC_DIR/*.c $APP_DIR/flipH.c -o $EXE_DIR/flipH

#	channel
gcc -Wall -std=c11 -I $HDR_DIR $SRC_DIR/*.c $APP_DIR/channel.c -o $EXE_DIR/channel

#	gray
gcc -Wall -std=c11 -I $HDR_DIR $SRC_DIR/*.c $APP_DIR/gray.c -o $EXE_DIR/gray

# dimensions
gcc -Wall -std=c11 -I $HDR_DIR $SRC_DIR/*.c $APP_DIR/dimensions.c -o $EXE_DIR/dimensions

#	split
gcc -Wall -std=c11 -I $HDR_DIR $SRC_DIR/*.c $APP_DIR/split.c -o $EXE_DIR/split

# compare
gcc -Wall -std=c11 -I $HDR_DIR $SRC_DIR/*.c $MAINAPP_DIR/compare.c -o $EXE_DIR/compare

# matchSearch
gcc -Wall -std=c11 -I $HDR_DIR $SRC_DIR/*.c $MAINAPP_DIR/matchSearch.c -o $EXE_DIR/matchSearch

echo "done."
