import numpy as np
from sklearn import tree
from sklearn.ensemble import AdaBoostClassifier
from sklearn.ensemble import RandomForestClassifier

movieData = []
movidData_f = open("./split_valid.txt", "r")
for line in movidData_f:
  attributes_s = line.split(' ')
  attrs= []
  for attr in attributes_s:
    attrs.append(int(attr))

  movieData.append(attrs)

movidData_f.close()

movieData = np.asarray(movieData)
X = movieData[:, 0:2]
Y = movieData[:, 3]

X1 = X[:1800000, :]
Y1 = Y[:1800000]
X2 = X[1800000:, :]
Y2 = Y[1800000:]

clf = tree.DecisionTreeClassifier()
clf= AdaBoostClassifier(n_estimators=400)
clf = RandomForestClassifier(min_samples_leaf = 20)
clf = clf.fit(X, Y)


print(clf.score(X2, Y2))

testData = []
testData_f = open("./split_qual.txt", "r")
for line in testData_f:
  attributes_s = line.split(' ')
  attrs= []
  for attr in attributes_s:
    attrs.append(int(attr))

  testData.append(attrs)

testData_f.close()

testData = np.asarray(testData)
T = testData[:, 0:2]
y = clf.predict(T)

result_f = open('result.dta', 'w')
for i in range(len(y)):
  result_f.write(str(float(y[i])) + '\n')

result_f.close()
