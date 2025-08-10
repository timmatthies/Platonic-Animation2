from PIL import Image
import glob
import os

out_dir = "png_imgs"

if not os.path.exists(out_dir):
    os.makedirs(out_dir)

cnt = 0
print("Converting BMP images to PNG format...")
for img in glob.glob('imgs/*.bmp'):
    Image.open(img).resize((400,400)).save(os.path.join(out_dir, f"{cnt:05d}" + '.png'))
    cnt += 1