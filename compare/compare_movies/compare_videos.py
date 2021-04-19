from malariaMovieReader import *

with open('c_movie.txt') as f:
    x = f.read()
c_images = map(lambda x: np.asfarray(x), map(lambda frame: map(lambda y: y.split("\t")[:-1], frame.split("\n")), x.split("\n\n")[:-1]))

m = Movie('/media/gn299/malaria1/data/feedback_fixed_3_video/cell_0004.movie')
for i, c_frame in enumerate(c_images):
	print np.sqrt(np.mean((m.get_frame(i) - c_frame)**2))
