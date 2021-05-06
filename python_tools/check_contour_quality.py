import numpy as np
from malariaMovieReader import *
import sys

f = "/home/gn299/data/malaria1/data/kate_stuff/kate_control/cell_0000_contour_full.txt"


#with open(sys.argv[1]) as g:
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

def plot(i):
	#plt.plot([position_vectors[i][0]], [position_vectors[i][1]], c = 'r', marker = 'o')
	plt.plot(*contour_fine_list[i][d['start'][i]:].T, c = 'r', marker = '')
	plt.axis('equal')

bad_i = np.asarray(np.argwhere([i == -1 for i in max_is]))
bad_i = [b[0] for b in bad_i]
print len(bad_i)

angles = []
angles_px = []
for i in range(len(contour_fine_list)):
	if d['start'][i] == -1:
		continue
	else:
		z = contour_fine_list[i][d['start'][i]:]
		z_px = contour_px_list[i][d['start'][i]:].astype('float')
		dd = np.diff(z, axis = 0)
		dd_px = np.diff(z_px, axis = 0)
		angles.append([i, np.arccos( np.sum(dd[1:]*dd[:-1], axis = 1)/(np.linalg.norm(dd[1:], axis = 1)*np.linalg.norm(dd[:-1], axis = 1)) )*180./np.pi])
		angles_px.append([i, np.arccos( np.sum(dd_px[1:]*dd_px[:-1], axis = 1)/(np.linalg.norm(dd_px[1:], axis = 1)*np.linalg.norm(dd_px[:-1], axis = 1)) )*180./np.pi])

