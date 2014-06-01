#!d:/python33/python.exe

import numpy as np
import pylab as pl

def load(fname):
  ''' load the file using std open'''
  f = open(fname, 'r')

  data = []
  for line in f.readlines():
    data.append(line.replace('\n', '').split(' '))

  f.close()

  return data


#path="C:/Users/PC/Desktop/python/log/t_hello.log"
#path="C:/Users/PC/Desktop/python/log/w1_hello.log"
#path="C:/Users/PC/Desktop/python/log/528/jqhello2.log"
path="c:/users/pc/desktop/hello.log" 

data = np.loadtxt(path)

#data[:,1] = data[:,1] - 100

pl.plot(data[:,0], data[:,1], 'ro')
pl.xlabel('time')
pl.ylabel('freq')
pl.xlim(0.0, 2000.)
pl.show()
