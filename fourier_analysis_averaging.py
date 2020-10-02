import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import poisson
from scipy import integrate

#Create an oversampled signal angle, R
oversampled_n = 10000
modes = oversampled_n*[0]
modes[0] = 4000
modes[5] = 50
modes[6] = 60
modes[7] = 70
modes[8] = 80
modes[9] = 90
angle = np.linspace(0, 2*np.pi, oversampled_n)
R = np.sum([m*np.cos(n*angle) for n,m in enumerate(modes)], axis = 0)

#Sumsample unevenly
sample_n = 150
sample_points = [poisson(int(len(R)/sample_n)).rvs()]
while np.cumsum(sample_points)[-1] < len(R) - 1:
	sample_points.append(poisson(int(len(R)/sample_n)).rvs())
sample_points = [0] + list(np.cumsum(sample_points)[:-1]) + [-1]

#x = R*np.cos(angle)
#y = R*np.sin(angle)
#
#x_s = [x[s] for s in sample_points]
#y_s = [y[s] for s in sample_points]
#
#v = np.column_stack((x, y))
#v_s = np.column_stack((x_s, y_s))
#
#R_s = np.linalg.norm(v_s, axis = 1)
#
#v_s -= v_s.mean(axis = 0) #TODO The mean value changes a lot with uneven sampling!
#angle_s = np.arctan2(v_s[:,1], v_s[:,0])
#angle_s[angle_s < 0] += 2*np.pi

angle_s = np.asfarray([angle[i] for i in sample_points])
R_s = np.asfarray([R[i] for i in sample_points])

#print angle_s[:2], angle_s[-2:] - 2*np.pi
#print R_s[:2], R_s[-2:]

#######################

#Extract modes from original signal
#First mode is a/2...
ex_modes = np.abs(np.fft.rfft(R)[:10])/float(oversampled_n)
ex_modes[1:] *= 2

ex_modes_s = np.abs(np.fft.rfft(R_s)[:10])/float(len(R_s))
ex_modes_s[1:] *= 2

ex_modes_si = np.abs(np.asfarray([np.sum(np.cos(float(n)*angle_s)*R_s) for n in range(10)]))/float(len(R_s))
ex_modes_si[1:] *= 2

ex_modes_i = np.abs(np.asfarray([np.sum(np.cos(float(n)*angle)*R) for n in range(10)]))/float(len(R))
ex_modes_i[1:] *= 2

#ex_modes_ss = np.abs(np.asfarray([integrate.simps(angle_s, np.exp(-1j*float(n)*angle_s)*R_s) for n in range(10)]))/(2*np.pi)
#ex_modes_ss[1:] *= 2

def slow_fft(angle_s, R_s, max_mode = 20):
        ex_modes_trap_a = []
        ex_modes_trap_b = []
        for n in range(max_mode + 1):
                integrand_a = np.cos(float(n)*angle_s)*R_s
                integrand_b = np.sin(float(n)*angle_s)*R_s
                ex_modes_trap_a.append(np.sum(np.diff(angle_s)*0.5*(integrand_a[1:] + integrand_a[:-1])) / (2*np.pi) )
                ex_modes_trap_b.append(np.sum(np.diff(angle_s)*0.5*(integrand_b[1:] + integrand_b[:-1])) / (2*np.pi) )
        ex_modes_trap = np.column_stack((ex_modes_trap_a, ex_modes_trap_b))
        ex_modes_trap[1:] *= 2
        return ex_modes_trap

ex_modes_trap = []
for n in range(10):
	integrand = np.cos(float(n)*angle_s)*R_s
	ex_modes_trap.append(np.sum(np.diff(angle_s)*0.5*(integrand[1:] + integrand[:-1])) / (2*np.pi) )
ex_modes_trap = np.asfarray(ex_modes_trap)
ex_modes_trap[1:] *= 2

print ex_modes_trap
ex_modes_trap2 = slow_fft(angle_s, R_s)
print np.linalg.norm(ex_modes_trap2, axis = 1)[:10]

#plt.plot(modes[1:10], label = 'real modes', marker = 'o')
plt.plot(ex_modes[1:] - modes[1:10], label = 'oversampled modes (perfect)', marker = 'o')
plt.plot(ex_modes_s[1:] - modes[1:10], label = 'sampled modes (bad)', marker = 'o')
#plt.plot(ex_modes_si[1:], label = 'integrated, subsampled modes', marker = 'o')
plt.plot(ex_modes_i[1:] - modes[1:10], label = 'integrated, oversampled modes (bad)', marker = 'o')
#plt.plot(ex_modes_trap[1:] - modes[1:10], label = 'trap, subsampled modes', marker = 'o')
plt.plot(np.linalg.norm(ex_modes_trap2, axis = 1)[1:10] - modes[1:10], label = 'func', marker = 'o')
plt.legend()
plt.show()
exit()

#######################

plt.figure('circle')
plt.plot(x,y)
plt.plot(x_s, y_s, marker = 'o', linestyle = '')
plt.axis('equal')

plt.figure('R')
plt.plot(angle_s, R_s, marker = 'o', linestyle = '')
plt.plot(angle, R)

plt.figure('modes')
m = np.abs(np.fft.rfft(R))[:10]
m_s = np.abs(np.fft.rfft(R_s))[:10]

m /= m[0]
m_s /= m_s[0]
plt.plot( m[1:], label = 'Signal')
plt.plot( m_s[1:], label = 'Subsampled faast')
plt.legend()


plt.show()
