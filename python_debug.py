from malariaMovieReader import *
from contour_tracker import *
import matplotlib.pyplot as plt

m  = Movie("/Users/guilherme/Desktop/cell_images/cell_0001.movie")
image = m.get_frame(0)
plt.figure('Image')
plt.imshow(image)

center = [112, 112]

#axis = 'x'
#c = [150, 115]
#axis = 'x'
#c = [75, 115]

#axis = 'y'
#c = [113, 78]

#axis = 'v'
#c = [86, 86]

axis = 'w'
c = [136, 85]

print max_slope(image, c, axis, center, plot = True, verbose = False, debug = False, min_slope = 1., slope_window = 4, horizontal_window = 5)
plt.show()
