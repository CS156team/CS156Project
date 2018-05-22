


import numpy as np
import os



def file_to_vec(probefile, testfile,A,T):
	probe = np.loadtxt(probefile)
	test = np.loadtxt(testfile)
	np.shape(A)
	if A.shape[0] == 0:
		A = probe
		T = test
	else:
		A = np.vstack([A, probe])
		T = np.vstack([T, test])
	return A,T

A = np.array([])
T = np.array([])
truth = np.loadtxt("probetruth.dta")


i=1
probefile = "predictions/probepred" + str(i) + ".dta"
testfile = "predictions/testpred" + str(i) + ".dta"
while (os.path.isfile(probefile) and os.path.isfile(testfile)):
	A,T = file_to_vec(probefile, testfile,A,T)
	i+=1
	probefile = "predictions/probepred" + str(i) + ".dta"
	testfile = "predictions/testpred" + str(i) + ".dta"


A = np.transpose(A)
T = np.transpose(T)
weights = np.dot(np.dot(np.linalg.inv(np.dot(np.transpose(A),A)),np.transpose(A)),truth)
submit = np.dot(T,weights)
print "size of submission: " + str(submit.shape)
np.savetxt("predictions/submit.dta", submit, fmt='%f')




