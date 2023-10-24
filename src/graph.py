import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

#specify figure size (width, height)
fig = plt.figure(figsize=(8,3))
ax = fig.gca()

data = pd.read_csv('cyclictestURJC.csv')
data2 = pd.read_csv('cyclictestURJC1.csv')
data3 = pd.read_csv('cyclictestURJC2.csv')

# data['LATENCIA'] = data['LATENCIA'] / 1000000
# data['LATENCIA'].hist(bins=60, ax=ax, color='blue', edgecolor='black', range=[0, 11])

# TODO: only for tests change to above
data['LATENCIA'] = data['LATENCIA'] / 100
data['LATENCIA'].hist(bins=100, ax=ax, range=[0, 50], color='blue', alpha=0.5, edgecolor='black', label="Non Real Time - idle")
data2['LATENCIA'] = data2['LATENCIA'] / 100
data2['LATENCIA'].hist(bins=100, ax=ax, range=[0, 50], color='red', alpha=0.5, edgecolor='black', label="Non Real Time - hackbench")
data3['LATENCIA'] = data3['LATENCIA'] / 100
data3['LATENCIA'].hist(bins=100, ax=ax, range=[0, 50], color='green', alpha=0.5, edgecolor='black', label="Non Real Time - bonnie++")
plt.legend(loc="upper right")
plt.ylabel('Frecuencia')
plt.xlabel('latencia (microsegundos)')
plt.title('cyclictestURJC')
plt.show()