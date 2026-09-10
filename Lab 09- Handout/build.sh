#!/bin/bash
#
#	Just a simple bash script to build executables for this lab
#
#	I don't try to go fancy.  I just hard-code my commands.

SRC_DIR="ImageLibrary/src"
HDR_DIR="ImageLibrary/include"
APP_DIR="ImageLibrary/applications"
EXE_DIR="ImageLibrary/executables"

#	flipV
g++ -Wall -std=c++20 -I $HDR_DIR $SRC_DIR/ImageIO_TGA.cpp $SRC_DIR/ImageIO.cpp $SRC_DIR/flip.cpp \
                      $SRC_DIR/RasterImage.cpp $SRC_DIR/utilities.cpp $APP_DIR/flipVApp.cpp \
                      -o $EXE_DIR/flipV

#	flipH
g++ -Wall -std=c++20 -I $HDR_DIR $SRC_DIR/ImageIO_TGA.cpp $SRC_DIR/ImageIO.cpp $SRC_DIR/flip.cpp \
                      $SRC_DIR/RasterImage.cpp $SRC_DIR/utilities.cpp $APP_DIR/flipHApp.cpp \
                      -o $EXE_DIR/flipH

#	channel
g++ -Wall -std=c++20 -I $HDR_DIR $SRC_DIR/ImageIO_TGA.cpp $SRC_DIR/ImageIO.cpp $SRC_DIR/channel.cpp \
                      $SRC_DIR/RasterImage.cpp $SRC_DIR/utilities.cpp $APP_DIR/channelApp.cpp \
                      -o $EXE_DIR/channel

#	gray
g++ -Wall -std=c++20 -I $HDR_DIR $SRC_DIR/ImageIO_TGA.cpp $SRC_DIR/ImageIO.cpp $SRC_DIR/gray.cpp \
                      $SRC_DIR/RasterImage.cpp $SRC_DIR/utilities.cpp $APP_DIR/grayApp.cpp \
                      -o $EXE_DIR/gray

#-----------------------------
#	New threaded apps
#-----------------------------

#	gray
g++ -Wall -std=c++20 -I $HDR_DIR $SRC_DIR/ImageIO_TGA.cpp $SRC_DIR/ImageIO.cpp $SRC_DIR/gray_thr.cpp \
                      $SRC_DIR/RasterImage.cpp $SRC_DIR/utilities.cpp $APP_DIR/grayApp_thr.cpp \
                      -o $EXE_DIR/gray_thr

#	flipV
g++ -Wall -std=c++20 -I $HDR_DIR $SRC_DIR/ImageIO_TGA.cpp $SRC_DIR/ImageIO.cpp $SRC_DIR/flip_thr.cpp \
                      $SRC_DIR/RasterImage.cpp $SRC_DIR/utilities.cpp $APP_DIR/flipVApp_thr.cpp \
                      -o $EXE_DIR/flipV_thr

echo "done."


