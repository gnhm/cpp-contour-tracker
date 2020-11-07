import subprocess
import os
from contour_tracker import *
from malariaMovieReader import *

dirs = '/home/gn299/data/malaria1/data/feedback_fixed_3_video'
mf = os.path.join(dirs, 'cell_0000.movie')
full_cf = os.path.join(dirs, 'cell_0000_contour_full.txt')
#full_c = load_contour_cpp(full_cf)

int_to_ax = dict(zip(range(4), ['x','y','v','w']))
ax_to_int = dict(zip(['x','y','v','w'], range(4) ))

horizontal_window = 5

m = Movie(mf)
image = m.get_frame(0)
cL = [[30,30]]
r = []
p_p_L = []
c_p_L= []
points = []
axes = []
for x in range(image.shape[0])[::10]:
	print x
	for y in range(image.shape[1])[::10]:
		c = [x,y]
		for axis, v in unit_vectors.items():
			p_p = profile(image, c, axis, horizontal_window = horizontal_window)
			c_r = subprocess.check_output(['./profile', mf, '{:d}'.format(horizontal_window), '{:f}'.format(c[0]), '{:f}'.format(c[1]), "{:d}".format(ax_to_int[axis])])
			c_p = np.asfarray(c_r.split('\t')[:-1]).reshape(3,-1).T
			c_p_L.append(c_p)
			p_p_L.append(p_p)
			points.append([x,y])
			axes.append(axis)
			if len(c_p) == len(p_p):
				r.append( np.any(p_p - c_p))
print r
