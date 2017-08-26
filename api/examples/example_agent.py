"""
This Notebook shows som examples that can be useful for a neural agent.
"""
import beeview
import numpy as np
from PIL import Image
import matplotlib.pyplot as plt

renderer = beeview.Renderer("..\\..\\data\\hessen_2016\\scene.obj",
                            "..\\..\\data\\ommatidia.csv",True)

# render and plot a beeview
renderer.position = [0,-80,0]
renderer.direction = [0,0,-1]
img = renderer.render()
plt.imshow(img)
plt.show()

# render panoramic image as flat array with agent function
renderer.mode = renderer.Panoramic
pano = renderer.render_agent()
print(pano.shape)

# reshape the array for display and plot
img = pano.reshape(renderer.get_image_size()[1],renderer.get_image_size()[0],3)
plt.imshow(img)
plt.show()

# back to bee eye mode
renderer.mode = renderer.BeeEye

# bee_eye_data is a tuple with data for left and right eye 
bee_eye_data = renderer.render_agent()

left_eye = bee_eye_data[0]
right_eye = bee_eye_data[1]

# print data for first 20 ommatidia of left eye
row_format ="{:>20}{:>20}{:>50}"
print(row_format.format(*["elevation","azimuth","color"]))
for i in range(0,20):
    print(row_format.format(*[str(left_eye["azimuth"][i]),str(left_eye["elevation"][i]),str(left_eye["color"][i])]))

# measure distance to next object in down direction
height = renderer.measure_distance(renderer.position,[0,-1,0])
print("Current height: " + str(height))

# set position to be 5m above ground
renderer.position = [renderer.position[0], renderer.position[1] - height + 5, renderer.position[2]]
print("New height: " + str(renderer.measure_distance(renderer.position,[0,-1,0])))

# move agent 1 m to right and 1 m forward and set direction to moving direction
old_pos = renderer.position
renderer.position = [renderer.position[0]+1, renderer.position[1], renderer.position[2]+1]

renderer.direction = np.array(old_pos) - np.array(renderer.position)

img = renderer.render()
plt.imshow(img)
plt.show()