import numpy as np
import os

lyrics = """Oh cube,\n rocky little cube.\n You tile my heart,\n cute little cube.\n Thetra, spiky hot thetra.\n As methane, you are my flame.\n My platonic friends I love.\n Only five I know thereof.\n My platonic friends, I’m most proud of.\n Oh, my platonic love.\n Oh Octa, you beautiful breeze.\n Your six corners shine like a diamond.\n Oh Ico, my infinite Ocean.\n 20 faces, a perfect dice for me.\n Oh Dodecahedron , my universe.\n Your thirty edges are golden to me.\n My platonic friends I love.\n Only five I know thereof.\n My platonic friends, I’m most proud of.\n Oh, my platonic love.\n My platonic friends I love.\n Only five I know thereof.\n My platonic friends, I’m most proud of.\n Oh, my platonic love."""

split_lyrics = lyrics.split(" ")
num_lines = len(split_lyrics)
duration = 166.766

timings = [12.189, 12.708 15.545, 16.514, 16.964]

fps = 60
frames = int(duration * fps)

curr_text = ""
for i in range(len(frames)):
    t = i/fps
    # generate the subtitle text for each frame.
    # get the index of the current word.
    if t <= timings[0]:
        curr_text += split_lyrics[0] + " "
        timings.pop(0)
        split_lyrics.pop(0)
