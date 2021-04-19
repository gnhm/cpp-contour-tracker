import subprocess
from malariaMovieReader import *
from contour_tracker import *

moviefile = "/media/gn299/malaria1/data/feedback_fixed_3_video/cell_0004.movie"

m = Movie(moviefile)
image = m.get_frame(0)

first_point = np.asfarray([75., 113.])
center = np.asfarray([112., 112.])
chirality = 1
horizontal_window = 5
slope_window = 4

n = 4

args = [moviefile]
args.append("{:f}".format(first_point[0]))
args.append("{:f}".format(first_point[1]))
args.append("{:f}".format(center[0]))
args.append("{:f}".format(center[1]))
args.append("{:d}".format(horizontal_window))
args.append("{:d}".format(slope_window))
args.append("{:d}".format(chirality))
args.append("{:d}".format(n+1))


print "C:"
cc =  subprocess.check_output(['./get_contour'] + args)
print cc
c_f = np.asfarray(map(lambda x: x.split("\t"), cc.split("\n")[:-1]))
p_px, p_f = get_contour(image, first_point, center, end_points = None, N = 1000, burn = 10, plot = False, verbose = False, debug = False, slope_window = 4, horizontal_window = 5, adjust_edge = False)
print len(c_f)
print len(p_f)
#print np.mean(c_fine - p_f)
