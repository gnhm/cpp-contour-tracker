import numpy as np
from malariaMovieReader import *

f = "/Users/guilherme/data/feedback_fixed_3_video/cell_0001_contour_full.txt"

with open(f) as g:
	x = g.readlines()

contour_fine_mode = False
contour_px_mode = False

centers = []
position_vectors = []

contour_fine_list = []
contour_fine = []
max_is = []

d = dict()

tags = ['max_i', 'start', 'burn', 'horizontal_window', 'slope_window', 'chirality']
for tag in tags:
	d[tag] = []

for l in x:
	if 'Center' in l:
		c = l.split('(')[1].split(', ')
		centers.append([c[0], c[1].split(")\n")[0]])
	if 'Position vector' in l:
		c = l.split('(')[1].split(', ')
		position_vectors.append([c[0], c[1].split(")\n")[0]])
	for k, v in d.items():
		if k in l:
			v.append(int(l.split(' = ')[1]))
	if 'max_i' in l:
		max_is.append(int(l.split(' = ')[1]))
	if 'max_i' in l:
		max_is.append(int(l.split(' = ')[1]))

	if '<contour_fine>' in l:
		if contour_fine_mode == True:
			raise Exception
		else:
			contour_fine_mode = True
		continue
	if contour_fine_mode:
		if '</contour_fine>' in l:
			contour_fine_mode = False
			contour_fine_list.append(np.asfarray(contour_fine))
			contour_fine = []
		else:
			contour_fine.append(l.split('\t'))

centers = np.asfarray(centers)
position_vectors = np.asfarray(position_vectors)

m = Movie('/Users/guilherme/data/feedback_fixed_3_video/cell_0000.movie')

def plot(i):
	im = m.get_frame(i+1)
	plt.imshow(im)
	plt.plot([position_vectors[i][0]], [position_vectors[i][1]], c = 'r', marker = 'o')
	plt.plot(*contour_fine_list[i].T, c = 'r', marker = '')

	
bad = np.asarray(np.argwhere([i == -1 for i in max_is]))
bad = [b[0] for b in bad]
print len(bad)
