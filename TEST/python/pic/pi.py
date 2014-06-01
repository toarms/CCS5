#!d:/python33/python.exe

import matplotlib.pyplot as plt 
# quants: GDP 
# labels: country name 

path="c:/users/pc/desktop/python/major_country_gdp.txt" 
labels = [] 
quants = [] 

fh = open(path, 'r')

# Read data 
for line in fh.readlines(): 
  info = line.split() 
  labels.append(info[0]) 
  quants.append(float(info[1])) 

# make a square figure 
plt.figure(1, figsize=(6,6)) 

# For China, make the piece explode a bit 
def explode(label, target='China'): 
  if label == target: return 0.1 
  else: return 0 
  expl = map(explode,labels) 

# Colors used. Recycle if not enough. 
colors = ["pink","coral","yellow","orange"] 
# Pie Plot 
# autopct: format of "percent" string; 
plt.pie(quants, explode=expl, colors=colors, labels=labels, autopct='%1.1f%%',pctdistance=0.8, shadow=True) 
plt.title('Top 10 GDP Countries', bbox={'facecolor':'0.8', 'pad':5}) 

  plt.show() 
