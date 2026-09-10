#!/bin/bash 

# Call the build script to build the programs
Scripts/build.sh

# First argument is a relative path to a folder containing record files (with the .dat extension)
folderPath="$1"
# Create a temporary text file to store the results
tmpfile=$(mktemp /tmp/my_script_temp.XXXXXX)

# Use the trap command to ensure the temporary file is deleted even if the script fails
trap 'rm -f "$tmpfile"' EXIT

# Read all the .dat files in the specified folder and store their paths
fileList=$(find "$folderPath" -type f -name "*.dat" | sort)

#count number of .dat files
fileCount=$(echo "$fileList" | wc -l)

#add count to tmpfile
echo "$fileCount" > "$tmpfile"

#add filenames to tmpfile
echo "$fileList" >> "$tmpfile"

# Read each file path from the temporary file and process it with prog02_3
./prog02_1 "$tmpfile" "$2" "$3" "$4" "$5" "$6" "$7" "$8"

# The temporary file will be automatically deleted when the script exits
# because of the trap command set earlier