import subprocess
from malariaMovieReader import *
from contour_tracker import *

moviefile = "/media/gn299/malaria1/data/feedback_fixed_3_video/cell_0004.movie"

for n in range(5000):
	print n
	m = Movie(moviefile)
	image = m.get_frame(n)
	offset = 100524*n
	args = [moviefile, "{:d}".format(offset)]
	c_image = np.asarray([line.split('\t')[:-1] for line in subprocess.check_output(['./get_image'] + args).split('\n')[:-1]], dtype = 'float')
	print np.sqrt(np.mean((c_image - image)**2))
	print

#print np.sqrt(np.mean((c_image - image)**2))
#plt.figure()
#plt.imshow(c_image - image)
#plt.show()
#
#plt.figure()
#plt.imshow(image)
#plt.figure()
#plt.imshow(c_image)

