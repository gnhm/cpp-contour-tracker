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
	print x
	for y in range(image.shape[1])[::5]:
		c = [x,y]
		for axis, v in unit_vectors.items():
			if axis == 'x':
				p_p = profile(image, c, axis, horizontal_window = horizontal_window)

				p_s = [linregress(p_p[:,0][i:window+i], p_p[:,2][i:window+i])[:2] for i in range(len(p_p) - window)] #Gets the Python running slope and intercept
				i_max = np.argmax([-orientation*s[0] for s in p_s]) #3.5us
				slope, intercept = p_s[i_max] #Gets the Python max slope and intercept

				args = ["{:d}".format(window), "{:d}".format(orientation)] + ["{:d}".format(xx) for xx in p_p[:,0]] + ["{:d}".format(xx) for xx in p_p[:,2]]
				c_r = subprocess.check_output(['./get_all_slopes'] + args) 
				slopes, intercepts = np.asfarray([z.split('\t')[:-1] for z in c_r.split('\n')]) #Gets the C running slope and intercept

				c_rm = subprocess.check_output(['./get_maximum_slope'] + args) #Gets the C maximum slope and point
				c_rm = np.asfarray(c_rm.split('\t')[:-1])

				python_slopes = np.asfarray([pp_ss[0] for pp_ss in p_s])
				python_intercepts = np.asfarray([pp_ss[1] for pp_ss in p_s])
			
				if not np.all(np.abs(python_slopes - slopes) < 1E-3):
					print "Slopes mismatch"
					print "Python slopes:"
					print python_slopes
					print "C slopes:"
					print slopes
				if not np.all(np.abs(python_intercepts - intercepts) < 1E-3):
					print "Intercepts mismatch"
					print "Python intercepts:"
					print python_intercepts
					print "C intercepts:"
					print intercepts

				python_max_slope = slope
				python_max_intercept = intercept
				c_i_max = np.argmax(-orientation*slopes)
				c_max_slope = slopes[c_i_max]
				c_max_intercept = intercepts[c_i_max]

				c_fast_max_slope, c_fast_max_intercept = c_rm

				if not np.abs(c_max_slope - python_max_slope) < 1E-3:
					print "Max slope mismatch"
					print "Python max slope:"
					print python_max_slope
					print "C max slope:"
					print c_max_slope

				if not np.abs(c_max_intercept - python_max_intercept) < 1E-3:
					print "Max intercept mismatch"
					print "Python max intercept:"
					print python_max_intercept
					print "C max intercept:"
					print c_max_intercept

				if not np.abs(c_max_slope - c_fast_max_slope) < 1E-3:
					print "Max slope mismatch"
					print "C-fast max slope:"
					print c_fast_max_slope
					print "C max slope:"
					print c_max_slope

				if not np.abs(c_max_intercept - c_fast_max_intercept) < 1E-3:
					print "Max intercept mismatch"
					print "C-fast max intercept:"
					print c_fast_max_intercept
					print "C max intercept:"
					print c_max_intercept
					print
					bad += 1

				total += 1
print total
print bad


#				print 
				
#				if (not -np.max(-np.asfarray(slopes)) == c_rm[0]) or (np.abs(intercept - c_rm[1]) > 0.1):
#					print c_rm
#					print min(slopes)
#					print slopes
#					print intercept
#					print c_rm
#					print p_s
#					print
#					print p_p
#					counter += 1
#					if counter == 1:
#						exit()
				
#				ps_L.append(np.asfarray(p_s)[:,0])
#				cs_L.append(slopes)
#				print np.all(np.abs(np.asfarray(p_s)[:,0] - slopes) < 1)
#				exit()

				#c_s = np.asfarray(c_r.split('\t')[:-1])
				#cs_L.append( c_s)
				#ps_L.append( [slope, intercept])
				#p_L.append(p_p)
#				points.append([x,y])
#				axes.append(axis)

#cs_L = np.asfarray(cs_L)
#ps_L = np.asfarray(ps_L)
