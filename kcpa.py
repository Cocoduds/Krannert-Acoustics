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
from scipy.fft import fft, fftfreq

plt.close('all')



df = pd.read_csv('10-10-2023.txt'); df
# spacing = (((df['time'].iloc[-1]-df['time'].iloc[0])*10**-6))
# rate = 1/((df['time'].iloc[-1]-df['time'].iloc[1])/len(df['time'])*10**-6)
# print(rate)
N=len(df['raw data'])
rate = 36488

f, t, Sxx = spectrogram(df['raw data'],rate, nfft=100)
plt.pcolormesh(t, f, Sxx, shading='gouraud')
plt.ylabel('Frequency [Hz]')
plt.xlabel('Time [sec]')
plt.show()

# vf = fft(np.array((df['voltage'])))
# xf = fftfreq(N, spacing)[:N//2]
# plt.plot(xf, np.abs(vf[:N//2])  )

# plt.plot(df['time'], df['voltage'])
# plt.show()
    



