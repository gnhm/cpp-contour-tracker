import sys
import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import linregress

data = sys.stdin.read()

data = data.split('\n')[:-1]
image = []
contour = []
mode = None
for line in data:
	if "<IMAGE>" in line:
		mode = 'image'
	elif "<CONTOUR>" in line:
		mode = 'contour'
	elif "</CONTOUR>" in line or "</IMAGE>" in line:
		mode = 'None'
	else:
		if mode == 'image':
			image.append(line.split('\t')[:-1])
		elif mode == 'contour':
			contour.append(line.split('\t'))

if image:
	image = np.asfarray(image)
	plt.imshow(image)
if contour:
	contour = np.asfarray(contour)
	plt.plot(*contour.T, c = 'r', marker = '')

plt.show()
