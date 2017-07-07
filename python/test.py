import bee_view
import numpy as np

pystring = "D:\\Documents\\bachelorarbeit\\raytracing\\beeView\\models\\hessen\\skydome_minus_z_forward.obj";
beeViewApp = bee_view.BeeViewPy(pystring);
test_img = beeViewApp.render();
