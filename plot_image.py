import sys
import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import linregress

data = sys.stdin.read()
data = np.asfarray([l.split('\t')[:-1] for l in data.split('\n')[:-1]])
plt.imshow(data)
plt.show()
#sw = 5
#print np.max([linregress(d[i:i+sw], data[i:i+sw,2])[0] for i in range(len(d) - sw)])
#plt.show()
