import subprocess
import os
from contour_tracker import *
from malariaMovieReader import *

dirs = '/home/gn299/data/malaria1/data/feedback_fixed_3_video'
mf = os.path.join(dirs, 'cell_0000.movie')

int_to_ax = dict(zip(range(4), ['x','y','v','w']))
ax_to_int = dict(zip(['x','y','v','w'], range(4) ))

horizontal_window = 20
slope_window = 5
center = [112, 112]

#There is a problem with 'v' and 'w'

total = 0
bad = 0

m = Movie(mf)
image = m.get_frame(0)
for x in range(image.shape[0])[30:-30:1]:
	for y in range(image.shape[1])[30:-30:1]:
		c = [x,y]
		for axis, v in unit_vectors.items():
			if axis == 'v' or axis == 'w':
				args = [mf]
				args.append( "{:d}".format(horizontal_window) )
				args.append( "{:f}".format(c[0]) )
				args.append( "{:f}".format(c[1]) )
				args.append( "{:d}".format(ax_to_int[axis]) )
				args.append( "{:d}".format(slope_window) )
				c_return = subprocess.check_output(['./max_slope'] + args)
				python_bar_point_v = max_slope(image, c, axis, center, slope_window = slope_window, horizontal_window = horizontal_window)
				python_bar_point_v = np.hstack(python_bar_point_v)

				c_bar_point_v = np.asfarray(c_return.split("\t")[:-1])
				total += 1
				if not np.all(abs(c_bar_point_v - python_bar_point_v) < 1):
					bad += 1
					print "bad = {:d}".format(bad)
					print "total = {:d}".format(total)
					print

					x_coordinates, y_coordinates, p = profile(image, c, axis, horizontal_window = horizontal_window).T #24.4us, quick
					orientation = -1 if np.sum((np.asfarray(center) - c)*unit_vectors[axis]) < 0 else 1
					unit_vector = unit_vectors[axis]*orientation
					d = np.sum((np.column_stack((x_coordinates, y_coordinates)) - c)*unit_vector, axis = 1)
					slopes = [linregress(d[i:slope_window+i], p[i:slope_window+i])[:2] for i in range(len(p) - slope_window)]
					i_max = np.argmax([-s[0] for s in slopes]) #3.5us
					slope, intercept = slopes[i_max]
					print len([s for s in slopes if abs(s[0] - slope) < 1])

print bad
print total
