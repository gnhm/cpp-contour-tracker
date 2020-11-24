import subprocess
from malariaMovieReader import *
from contour_tracker import *

moviefile = "/media/gn299/malaria1/data/feedback_fixed_3_video/cell_0004.movie"

m = Movie(moviefile)
image = m.get_frame(0)
args = [moviefile]
c_image = np.asarray([line.split('\t')[:-1] for line in subprocess.check_output(['./get_image'] + args).split('\n')[:-1]], dtype = 'float')
plt.figure()
plt.imshow(image)
plt.figure()
plt.imshow(c_image)
plt.figure()
plt.imshow(c_image - image)
plt.show()
