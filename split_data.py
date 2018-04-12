import numpy as np

#movieData = np.loadtxt('./testdata.dta', dtype = 'str', delimiter = ' ')
#index = np.loadtxt('./index.txt', dtype = 'str', delimiter = ' ')

movieData = []
index = []

#Do not read all this data into memory.
movidData_f = open("./all.dta", "r")
  
#this is less bad, since the file is smaller, but still
index_f = open("./all.idx", "r")
for line in index_f:
  index.append(line.strip("\n"))
  
base = open("split_base.txt","w+")
valid = open("split_valid.txt","w+")
hidden = open("split_hidden.txt","w+")
probe = open("split_probe.txt","w+")
qual = open("split_qual.txt","w+")

#from googling "how to read to files at the same time python", I found code 
#like this:

#with open(file1) as f1, open(fil2) as f2:
#  for x, y in zip(f1, f2):
#     do stuff(x,y)

#this seems like the best way to accomplish this without reading 2 gigabytes
#into memory, but this should make the memory issue much less bad

i = 0
for line in movidData_f:
  idx = index[i]
  if idx=="1" :
    base.write(line)
  elif idx=="2" :
    valid.write(line)
  elif idx=="3" :
    hidden.write(line)
  elif idx=="4" :
    probe.write(line)
  else:
    qual.write(line)
  i = i+1;

base.close() 
valid.close()
hidden.close()
probe.close()
qual.close()
