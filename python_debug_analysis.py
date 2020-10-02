import numpy as np
from contour_tracker import *
import matplotlib.pyplot as plt
import os

machine_name = os.uname()[1]
if machine_name == "argonauta":
	test_file = "/Users/guilherme/data/cell_0000_contour_full.txt"
elif machine_name == "pc104":
	test_file = "/home/gn299/data/malaria1/data/kate_stuff/kate_control/cell_0000_contour_full.txt"

c = load_contour_cpp(test_file)
#How many good contours are there?
#What are the ways contours can be bad?
#What is the difference between the pecreaux and averaging modes?

min_mode = 5
max_mode = 12

def R(x):
	ds = np.linalg.norm(np.diff(x, axis = 0), axis = 1)
	L = ds.sum()
	return L/(2*np.pi)

def modes(x):
	ds = np.linalg.norm(np.diff(x, axis = 0), axis = 1)
	L = ds.sum()
	x_c = np.vstack([(ds[i-1] + ds[i])*x[i] for i in range(len(x) - 1)] + [(ds[len(x) - 2] + ds[0])*x[len(x) - 1]]).sum(axis = 0)/(2*L) #The center. Good so far

	xt = x - x_c
	r = np.linalg.norm(xt, axis = 1)
	angle = np.arctan2(*xt.T)

	#TODO Here's the problem! If we resort, then we have a zero difference in the middle!
	r = r[:-1][np.argsort(angle[:-1])]
	angle = angle[:-1][np.argsort(angle[:-1])]

	angle = np.hstack((angle, angle[0] + 2*np.pi))
	r = np.hstack((r, r[0]))

	r0 = 0.5*(r[:-1] + r[1:])*np.diff(angle)/(2*np.pi) #TODO We 
	a = [np.sum((r[:-1]*np.cos(n*angle[:-1]) + r[1:]*np.cos(n*angle[1:]))*np.diff(angle)*0.5)/(np.pi*r0) for n in range(min_mode, max_mode + 1)]
	b = [np.sum((r[:-1]*np.sin(n*angle[:-1]) + r[1:]*np.sin(n*angle[1:]))*np.diff(angle)*0.5)/(np.pi*r0) for n in range(min_mode, max_mode + 1)]
	c = np.linalg.norm(np.column_stack((a,b)), axis = 1)
	R = L/(2*np.pi)
	return c

def dots(x):
	d = np.diff(x, axis = 0)
	dd = np.sum(d[:-1]*d[1:], axis = 1)/(np.linalg.norm(d[:-1], axis = 1)*np.linalg.norm(d[1:], axis =1))
	return np.min(dd)

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

def polar(x):
	x_c = x - x.mean(axis = 0)
	R = np.linalg.norm(x_c, axis = 1)
	angle = np.arctan2(x_c[:,1], x_c[:,0])

	R = R[:-1][np.argsort(angle[:-1])]
	angle = angle[:-1][np.argsort(angle[:-1])]

	angle = np.hstack((angle, angle[0] + 2*np.pi))
	R = np.hstack((R, R[0]))
	return angle, R

m = []
bad = []
dot_L = []
perfect = []
for i, cc in c:
	try:
		angle, R = polar(cc)
		mode = slow_fft(angle, R)
		if np.all(np.isfinite(mode)):
			m.append([i, mode])
			dot_product = dots(cc)
			dot_L.append([i, dot_product])
			if dot_product > 0.8:
				perfect.append([i, mode])
		else:
			bad.append(i)
	except:
		bad.append(i)

#perfect_modes = np.asfarray([mm for i,mm in perfect])
