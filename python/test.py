import beeview
import numpy as np
from PIL import Image
from pprint import pprint

scene = "D:\\Documents\\bachelorarbeit\\bee_view\\data\\sky_white\\skydome_white.obj";
ommatidia = "D:\\Documents\\bachelorarbeit\\bee_view\\data\\ommatidia.csv";

bee_view_api = beeview.Renderer(scene, ommatidia, True)
pos = [0,-70,0]
bee_view_api.set_camera_position(pos)
dir = [0,0,1]
bee_view_api.set_camera_dir(dir)

im = bee_view_api.render()
im.save('test_api.ppm')