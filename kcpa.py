# -*- coding: utf-8 -*-
"""
Created on Thu Sep  7 16:18:23 2023

@author: dudle
"""


import numpy as np
from scipy.optimize import curve_fit
import matplotlib.pyplot as plt
import pandas as pd
from scipy.signal import spectrogram

plt.close('all')



df = pd.read_csv('DATA.txt'); df
print(((df['time'].iloc[-1]-df['time'].iloc[0])*10**-6))
rate = 1/((df['time'].iloc[-1]-df['time'].iloc[1])/len(df['time'])*10**-6)

# f, t, Sxx = spectrogram(df['voltage'],rate)
# plt.pcolormesh(t, f, Sxx, shading='gouraud')
# plt.ylabel('Frequency [Hz]')
# plt.xlabel('Time [sec]')
# plt.show()

plt.plot(df['time'], df['voltage'])
plt.show()
    



