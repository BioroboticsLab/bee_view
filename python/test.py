import beeview
import numpy as np
from PIL import Image
from pprint import pprint

scene = "D:\\Documents\\bachelorarbeit\\raytracing\\beeView\\models\\hessen\\skydome_minus_z_forward.obj";
ommatidia = "D:\\Documents\\bachelorarbeit\\bee_view\\data\\ommatidia.csv";

bee_view_api = beeview.API(scene, ommatidia)
pos = [0,-70,0]
bee_view_api.set_camera_position(pos)
dir = [0,0,1]
bee_view_api.set_camera_dir(dir)

im = bee_view_api.render()
#test_img = (np.array(test_img) * 255).astype(np.uint8)
#im = Image.fromarray(test_img)
im.save('test2.ppm')