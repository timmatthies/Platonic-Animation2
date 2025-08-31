#!/bin/bash

# Set the base directory to the location of this script
BASE_DIR="$(dirname "$0")"

# Iterate over all subdirectories in the base directory
for dir in "$BASE_DIR"/*/; do
    # Remove trailing slash and get folder name
    folder_name=$(basename "$dir")
    
    # Execute PlatonicAnimation3.exe with the folder name
    PlatonicAnimation3.exe "$folder_name"
done

# Create a list of all animation_output.mp4 files
input_files=()
for dir in "$BASE_DIR"/*/; do
    mp4_file="${dir}imgs/animation_output.mp4"
    if [ -f "$mp4_file" ]; then
        input_files+=("$mp4_file")
    fi
done

# Create a temporary file list for ffmpeg
file_list=$(mktemp)
for f in "${input_files[@]}"; do
    echo "file '$f'" >> "$file_list"
done

# Combine all videos into one mp4
ffmpeg -f concat -safe 0 -i "$file_list" -c copy "$BASE_DIR/combined_animation.mp4"

# Clean up
rm "$file_list"