#!/bin/bash 

sourceFolder="$1"
searchFolder="$2"
outputFolder="$3"
# Call the build script to build the programs
Scripts/build.sh

#if outputFolder does not exist, create it
if [ ! -d "$outputFolder" ]; then
    mkdir -p "$outputFolder"
fi
#if it exists, delete all files and subfolders in it
if [ -d "$outputFolder" ]; then
    rm -r "$outputFolder"/*
fi
#look for files in sourceFolder with .tga extension
fileList=$(find "$sourceFolder" -name "*.tga")
# call matchSearch for each file found
for file in $fileList; do
    ImageLibrary/executables/matchSearch "$file" "$searchFolder" "$outputFolder"
done