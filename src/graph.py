import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import scienceplots

plt.style.use('science')
#specify figure size (width, height)
fig = plt.figure(figsize=(8,3))
ax = fig.gca()

ax.tick_params(axis='x', labelsize=28)
ax.tick_params(axis='y', labelsize=28)

data = pd.read_csv('noRtBonnie.csv')
data2 = pd.read_csv('noRtHackbench.csv')
data3 = pd.read_csv('noRtIdle.csv')

data['LATENCIA'] = data['LATENCIA'] / 1000000
data['LATENCIA'].hist(bins=100, ax=ax, range=[0.00, 0.05], color='blue', alpha=0.5, edgecolor='black', label="Non Real Time - bonnie++")
data2['LATENCIA'] = data2['LATENCIA'] / 1000000
data2['LATENCIA'].hist(bins=100, ax=ax, range=[0.00, 0.05], color='red', alpha=0.5, edgecolor='black', label="Non Real Time - hackbench")
data3['LATENCIA'] = data3['LATENCIA'] / 1000000
data3['LATENCIA'].hist(bins=100, ax=ax, range=[0.00, 0.05], color='green', alpha=0.5, edgecolor='black', label="Non Real Time - idle")

plt.legend(loc="upper right", prop={'size':30})
plt.ylabel('Frecuencia', fontsize=38)
plt.xlabel('latencia (microsegundos)', fontsize=38)
plt.title('cyclictestURJC RaspberryPi - Kernel No RT', fontsize=40)
plt.show()

fig = plt.figure(figsize=(8,3))
ax = fig.gca()

ax.tick_params(axis='x', labelsize=28)
ax.tick_params(axis='y', labelsize=28)

data = pd.read_csv('RtBonnie.csv')
data2 = pd.read_csv('RtHackbench.csv')
data3 = pd.read_csv('RtIdle.csv')

data['LATENCIA'] = data['LATENCIA'] / 1000000
data['LATENCIA'].hist(bins=100, ax=ax, range=[0.00, 0.05], color='blue', alpha=0.5, edgecolor='black', label="Real Time - bonnie++")
data2['LATENCIA'] = data2['LATENCIA'] / 1000000
data2['LATENCIA'].hist(bins=100, ax=ax, range=[0.00, 0.05], color='red', alpha=0.5, edgecolor='black', label="Real Time - hackbench")
data3['LATENCIA'] = data3['LATENCIA'] / 1000000
data3['LATENCIA'].hist(bins=100, ax=ax, range=[0.00, 0.05], color='green', alpha=0.5, edgecolor='black', label="Real Time - idle")

plt.legend(loc="upper right", prop={'size':30})
plt.ylabel('Frecuencia', fontsize=38)
plt.xlabel('latencia (microsegundos)', fontsize=38)
plt.title('cyclictestURJC RaspberryPi - Kernel RT', fontsize=40)
plt.show()