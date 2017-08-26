import beeview
from PIL import Image
import matplotlib.pyplot as plt

# an example scene and interommatidial angles file are in the data folder
renderer = beeview.Renderer("..\..\data\hessen_2016\scene.obj",
                            "..\..\data\ommatidia.csv",left_handed = True)

renderer.position = [0,-85,0]
renderer.direction = [0,0,-1]
img = renderer.render()

# plot hte image
plt.imshow(img)
plt.show()

# save the image
img = Image.fromarray(img)
img.save("beeview.png")

# change some render parameters
renderer.set_num_samples(56)
renderer.set_acceptance_angle(5.2)
renderer.set_ommatidium_size(8)

# and render + plot
renderer.mode = renderer.BeeEye
img = renderer.render()
plt.imshow(img)
plt.show()

# change the render mode, and render a panorama
renderer.mode = renderer.Panoramic
renderer.set_panoramic_hfov(200)
renderer.set_panoramic_vfov(90)
renderer.set_panoramic_width(800)

img = renderer.render()
plt.imshow(img)
plt.show()

# pinhole mode
renderer.mode = renderer.Pinhole
renderer.set_pinhole_fov(55)
renderer.set_pinhole_width(800)
renderer.set_pinhole_height(600)

img = renderer.render()
plt.imshow(img)
plt.show()

# measure distance from renderer to next object straight down
dist = renderer.measure_distance(renderer.position, [0,-1,0])
print(dist)