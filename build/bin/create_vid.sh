#!/bin/bash

# Set the base directory to the location of this script - use absolute path
BASE_DIR="$(cd "$(dirname "$0")" && pwd)"

# Set the base directory to the location of this script - use absolute path
BASE_DIR="$(cd "$(dirname "$0")" && pwd)"

# Iterate over all subdirectories in the base directory
for dir in "$BASE_DIR"/*/; do
    # Remove trailing slash and get folder name
    folder_name=$(basename "$dir")
    
    # Execute PlatonicAnimation3.exe with the folder name - use full path
    "$BASE_DIR/PlatonicAnimation3" "$folder_name"
done

# Create a list of all animation_output.mp4 files
input_files=()
for dir in "$BASE_DIR"/*/; do
    # Use full paths everywhere
    mp4_file="${dir}imgs/animation_output.mp4"
    if [ -f "$mp4_file" ]; then
        # Store absolute paths to avoid temporary directory issues
        input_files+=("$(realpath "$mp4_file")")
    fi
done

# Create a temporary file list for ffmpeg in the current directory
file_list="$BASE_DIR/temp_file_list.txt"
# Clear the file if it exists
> "$file_list"
# Add each file to the list
for f in "${input_files[@]}"; do
    echo "file '$f'" >> "$file_list"
done

# Combine all videos into one mp4
ffmpeg -f concat -safe 0 -i "$file_list" -c copy "$BASE_DIR/combined_animation.mp4"

# Clean up
rm "$file_list"