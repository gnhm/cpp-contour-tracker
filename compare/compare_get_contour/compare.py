import subprocess
from malariaMovieReader import *
from contour_tracker import *

moviefile = "/media/gn299/malaria1/data/feedback_fixed_3_video/cell_0004.movie"

m = Movie(moviefile)
image = m.get_frame(0)

first_point = np.asfarray([75., 113.])
center = np.asfarray([122., 112.])
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
print subprocess.check_output(['./get_contour'] + args)

print "Python"
contour = [ [int(first_point[0]), int(first_point[1])] ]
contour_fine = [list(np.asfarray(c)) for c in contour]
for i in range(n):
	next_point_fine, next_point_px = next_point(image, contour, center, plot = False, verbose = False, debug = False, slope_window = 4, horizontal_window = 5, chirality = chirality)
	contour.append(next_point_px)
	contour_fine.append(next_point_fine)
print contour
print contour_fine

exit()


#c = get_contour(image, first_point, center, end_points = None, N = 1000, burn = 10, plot = False, verbose = False, debug = False, slope_window = 4, horizontal_window = 5, adjust_edge = False)


#plt.plot(*c.T, c='r')
#plt.imshow(image)
#plt.show()
