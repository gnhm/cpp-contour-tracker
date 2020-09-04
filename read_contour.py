import numpy as np
from malariaMovieReader import *

f = "/Users/guilherme/data/feedback_fixed_3_video/cell_0000_contour_full.txt"

with open(f) as g:
	x = g.readlines()

contour_fine_mode = False
contour_px_mode = False

centers = []
position_vectors = []

contour_fine_list = []
contour_fine = []
contour_px_list = []
contour_px = []
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
	if '<contour_px>' in l:
		if contour_px_mode == True:
			raise Exception
		else:
			contour_px_mode = True
		continue
	if contour_px_mode:
		if '</contour_px>' in l:
			contour_px_mode = False
			contour_px_list.append(np.asarray(contour_px, dtype = 'int'))
			contour_px = []
		else:
			contour_px.append(l.split('\t'))

centers = np.asfarray(centers)
position_vectors = np.asfarray(position_vectors)

f_m = f.split('_contour_full.txt')[0] + '.movie'
m = Movie(f_m)

def plot(i):
	im = m.get_frame(i+1)
	plt.imshow(im)
	plt.plot([position_vectors[i][0]], [position_vectors[i][1]], c = 'r', marker = 'o')
	plt.plot(*contour_fine_list[i].T, c = 'r', marker = '')

	
bad = np.asarray(np.argwhere([i == -1 for i in max_is]))
bad = [b[0] for b in bad]
print len(bad)

#um = [not np.all(contour_px_list[i][-1] == contour_px_list[i][d['start'][i]]) if d['max_i'] != -1 else False for i in range(len(contour_px_list))]
#umf = [not np.all(contour_fine_list[i][-1] == contour_fine_list[i][d['start'][i]]) if d['max_i'] != -1 else False for i in range(len(contour_fine_list))]
#print "Unmatched start to last (px): ", np.mean(um)
print "Unmatched start to last (fine): ", np.mean(umf)

#for i in range(20):
#    if d['max_i'] != -1:
#        if not np.all(contour_fine_list[i][-1] == contour_fine_list[i][d['start'][i]]):
#            print i
#            plot(i)
#            plt.show()
