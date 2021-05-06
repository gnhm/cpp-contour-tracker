import sys
import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import linregress

data = sys.stdin.read()
data = np.asfarray([l.split('\t') for l in data.split('\n')[:-1]] )
#d = np.sqrt(np.sum((data[:,:2] - data[0, :2])**2, axis = 1))
print data.shape
plt.plot(*data.T)
plt.show()
#sw = 5
#print np.max([linregress(d[i:i+sw], data[i:i+sw,2])[0] for i in range(len(d) - sw)])
#plt.show()
