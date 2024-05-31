#!/bin/bash

# Check if a filename was provided as an argument
if [ $# -eq 0 ]; then
    echo "No filename provided. Usage: $0 <filename>"
    exit 1
fi

FILE=$1

if [ -f "particles/$FILE" ]; then
    echo "$FILE exists."
    cp particles/$FILE "particles1.dat"
else
    echo "particles/$FILE does not exist."
    # Perform the action if the file does not exist
fi
