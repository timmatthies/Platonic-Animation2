import numpy as np
import os
from PIL import Image, ImageDraw, ImageFont

lyrics = """Oh cube,\n rocky little cube.\n You tile my heart,\n cute little cube.\n Thetra, spiky hot thetra.\n As methane, you are my flame.\n My platonic friends I love.\n Only five I know thereof.\n My platonic friends, I’m most proud of.\n Oh, my platonic love.\n Oh Octa, you beautiful breeze.\n Your six corners shine like a diamond.\n Oh Ico, my infinite Ocean.\n 20 faces, a perfect dice for me.\n Oh Dodecahedron , my universe.\n Your thirty edges are golden to me.\n My platonic friends I love.\n Only five I know thereof.\n My platonic friends, I’m most proud of.\n Oh, my platonic love.\n My platonic friends I love.\n Only five I know thereof.\n My platonic friends, I’m most proud of.\n Oh, my platonic love."""

split_lyrics = lyrics.split(" ")
num_lines = len(split_lyrics)
duration = 166.766

timings = [12.189, 12.708, 15.545, 16.514, 16.964, 20.566, 21.296, 22.888]

fps = 60
frames = int(duration * fps)

curr_text = ""
break_next_line = False

def draw_text_on_frame(frame, text):
    img = Image.new("RGBA", (1080, 1080), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    font = ImageFont.truetype("arial.ttf", 36) 
    draw.text((540, 1000), text, font=font, fill=(232,213,242,255),align="center", anchor="ms")
    img.save(f"imgs/frame_{frame}.png")


if not os.path.exists("imgs"):
    os.makedirs("imgs")

for i in range(frames):
    t = i/fps
    # generate the subtitle text for each frame.
    # get the index of the current word.
    if t >= timings[0]:
        if curr_text != "" and break_next_line:
            curr_text = split_lyrics[0] + " "
            break_next_line = False
        else:
            curr_text += split_lyrics[0] + " "
            if "\n" in curr_text:
                break_next_line = True
                curr_text = curr_text.replace("\n", "")
        timings.pop(0)
        split_lyrics.pop(0)
        
        draw_text_on_frame(i, curr_text)
        if len(timings) == 0:
            break

    print(f"{t}: {curr_text}")

# Make a video from the generated frames
