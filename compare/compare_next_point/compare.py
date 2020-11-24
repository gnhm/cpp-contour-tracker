import subprocess
from malariaMovieReader import *
from contour_tracker import *

moviefile = "/media/gn299/malaria1/data/feedback_fixed_3_video/cell_0004.movie"

m = Movie(moviefile)
image = m.get_frame(0)

first_point = np.asfarray([76., 113.])
center = np.asfarray([122., 112.])
chirality = 1
horizontal_window = 5
slope_window = 4

#args = ["{:d}".format(window), "{:d}".format(orientation)] + ["{:d}".format(xx) for xx in p_p[:,0]] + ["{:d}".format(xx) for xx in p_p[:,2]]
args = [moviefile]
args.append("{:f}".format(first_point[0]))
args.append("{:f}".format(first_point[1]))
args.append("{:f}".format(center[0]))
args.append("{:f}".format(center[1]))
args.append("{:d}".format(horizontal_window))
args.append("{:d}".format(slope_window))
args.append("{:d}".format(chirality))

contour = [ [int(first_point[0]), int(first_point[1])] ]

print "C:"
print subprocess.check_output(['./next_point'] + args)

print "Python"
print contour
print next_point(image, contour, center, plot = False, verbose = False, debug = False, slope_window = 4, horizontal_window = 5, chirality = chirality)
exit()


#c = get_contour(image, first_point, center, end_points = None, N = 1000, burn = 10, plot = False, verbose = False, debug = False, slope_window = 4, horizontal_window = 5, adjust_edge = False)


#plt.plot(*c.T, c='r')
#plt.imshow(image)
#plt.show()
