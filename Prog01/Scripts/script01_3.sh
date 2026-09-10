#!/bin/bash 
Scripts/build.sh
pathList=$1
outputFile=$2
inputFiles=""
while IFS= read -r line; do
   inputFiles="$inputFiles $line"
done < "$pathList"  
./prog01v2 $inputFiles "$outputFile"
