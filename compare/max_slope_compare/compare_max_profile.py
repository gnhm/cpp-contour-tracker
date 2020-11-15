import subprocess
import os
from contour_tracker import *
from malariaMovieReader import *

dirs = '/home/gn299/data/malaria1/data/feedback_fixed_3_video'
mf = os.path.join(dirs, 'cell_0000.movie')

int_to_ax = dict(zip(range(4), ['x','y','v','w']))
ax_to_int = dict(zip(['x','y','v','w'], range(4) ))

horizontal_window = 20
slope_window = 4

m = Movie(mf)
image = m.get_frame(0)

center = [112., 112.]

#There are nans in the last bar_point_v_slope

#slopes are all good

points = []
axes = []
c_p_L = [] 
p_p_L = []

r = []
for x in range(image.shape[0])[horizontal_window:-horizontal_window:10]:
	for y in range(image.shape[1])[horizontal_window:-horizontal_window:10]:
		c = [x,y]
		for axis, v in unit_vectors.items():
			p_p = max_slope(image, c, axis, center, slope_window = slope_window , horizontal_window = horizontal_window )
			args = []
			args.append(mf)
			args.append('{:d}'.format(horizontal_window))
			args.append('{:f}'.format(c[0]))
			args.append('{:f}'.format(c[1]))
			args.append("{:d}".format(ax_to_int[axis]))
			args.append('{:f}'.format(center[0]))
			args.append('{:f}'.format(center[1]))
			args.append('{:d}'.format(slope_window))

			c_r = subprocess.check_output(['./max_profile'] + args)
			c_p = np.asfarray(c_r.split('\t')[:-1])
			if not np.all(np.abs(np.hstack((p_p[0], p_p[1])) - np.asfarray(c_p)) < 0.1):
				print c_p, p_p
				exit()
#			c_p = (c_p[:2], c_p[-1])
#			r.append([c_p[0] - p_p[0], c_p[1] - p_p[1]])
#			points.append([x,y])
#			c_p_L.append(c_p)
#			p_p_L.append(p_p)
#			axes.append(axis)
