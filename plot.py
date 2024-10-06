import numpy as np 
import matplotlib.pyplot as plt 
import matplotlib 

matplotlib.use('TkAgg')


d = np.fromfile("./sig.fft.bin", dtype = np.csingle)

plt.plot(np.abs(d.real))
plt.show() 

d = np.fromfile("./sig.dft.bin", dtype = np.csingle)

plt.plot(np.abs(d.real))
plt.show() 

