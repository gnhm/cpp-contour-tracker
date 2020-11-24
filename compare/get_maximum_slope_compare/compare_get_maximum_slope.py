import subprocess
import os
from contour_tracker import *
from malariaMovieReader import *

#TODO Stopped here on that terrible November Saturday. Occasionally c and python get max slope disagree. Fix this

dirs = '/home/gn299/data/malaria1/data/feedback_fixed_3_video'
mf = os.path.join(dirs, 'cell_0000.movie')
full_cf = os.path.join(dirs, 'cell_0000_contour_full.txt')
#full_c = load_contour_cpp(full_cf)

int_to_ax = dict(zip(range(4), ['x','y','v','w']))
ax_to_int = dict(zip(['x','y','v','w'], range(4) ))

horizontal_window = 20
window = 5
orientation = 1

center = [112, 112]

m = Movie(mf)
image = m.get_frame(0)
r = []
cs_L = []
ps_L= []
p_L = []
points = []
axes = []

counter = 0
total = 0
bad = 0
for x in range(image.shape[0])[::5]:
	#print x
	for y in range(image.shape[1])[::5]:
		c = [x,y]
		for axis, v in unit_vectors.items():
			if axis == 'x':
				p_p = profile(image, c, axis, horizontal_window = horizontal_window)
				p_s = [linregress(p_p[:,0][i:window+i], p_p[:,2][i:window+i])[:2] for i in range(len(p_p) - window)] #Gets the Python running slope and intercept
				i_max = np.argmax([-orientation*s[0] for s in p_s]) #3.5us
				python_max_slope, python_max_intercept = p_s[i_max] #Gets the Python max slope and intercept

				args = ["{:d}".format(window), "{:d}".format(orientation)] + ["{:d}".format(xx) for xx in p_p[:,0]] + ["{:d}".format(xx) for xx in p_p[:,2]]
				c_rm = subprocess.check_output(['./get_maximum_slope'] + args) #Gets the C maximum slope and point
				#[c_fast_max_slope, c_fast_max_intercept], c_fast_running_slopes, c_fast_running_intercepts = [np.asfarray(l.split("\t")[:-1]) for l in c_rm.split("\n")]
				c_fast_max_slope, c_fast_max_intercept = np.asfarray(c_rm.split("\t")[:-1])

				if not np.abs(python_max_slope - c_fast_max_slope) < 1E-3:
					print "Max slope mismatch"
					print "C-fast max slope:"
					print c_fast_max_slope
					print "Python max slope:"
					print python_max_slope
					bad += 1

				if not np.abs(python_max_intercept - c_fast_max_intercept) < 1E-3:
					print "Max intercept mismatch"
					print "C-fast max intercept:"
					print c_fast_max_intercept
					print "Python max intercept:"
					print python_max_intercept
					bad += 1
				total += 1
print total
print bad