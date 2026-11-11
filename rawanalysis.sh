#!/bin/bash

# Check if a file argument was provided and exists
if [ -z "$1" ] || [ ! -f "$1" ]; then
    echo "Usage: $0 <path/to/file>"
    exit 1
fi

# Read character by character (including newlines) using the file path argument
while IFS= read -r -n1 -d '' char; do
    if [ -z "$char" ]; then
        # Explicitly handle newline as ASCII 10
        printf "%d\n" 10
    else
        printf "%d\n" "'$char"
    fi
done < "$1"