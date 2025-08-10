#!/bin/bash

rm -rf imgs
mkdir imgs

./PlatonicAnimation.exe

rm -rf png_imgs

C:/Users/timma/AppData/Local/Microsoft/WindowsApps/python3.10.exe "c:/Users/timma/Documents/Platonic Animation/build/bin/convert_to_png.py"

./ffmpeg.exe -framerate 30 -i 'png_imgs/%5d.png' -c:v libx264 -pix_fmt yuv420p out.mp4 -y