#!/bin/bash
#
SRC_DIR="./ImageLibrary/src"
INCL_DIR="./ImageLibrary/include"
gcc -Wall -std=c99 -I $INCL_DIR $SRC_DIR/*.c  lab04V1.c -o ./version1
# gcc -Wall -std=c99 -I $INCL_DIR $SRC_DIR/*.c  lab04V2a.c -o ./version2a
# gcc -Wall -std=c99 -I $INCL_DIR $SRC_DIR/*.c  lab04V2b.c -o ./version2b
# gcc -Wall -std=c99 -I $INCL_DIR $SRC_DIR/*.c  lab04V3a.c -o ./version3a
# gcc -Wall -std=c99 -I $INCL_DIR $SRC_DIR/*.c  lab04V3b.c -o ./version3b

echo "done."