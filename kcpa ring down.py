# -*- coding: utf-8 -*-
"""
Created on Thu Sep  7 16:18:23 2023

@author: dudle

This program finds the ringdown time of a specified frequency
"""


import numpy as np
from scipy.optimize import curve_fit
from scipy.optimize import differential_evolution
import warnings
import matplotlib.pyplot as plt
import pandas as pd
from scipy.signal import spectrogram
from scipy.fft import fft, fftfreq

plt.close('all')

Fq = 440 #this is if you dont want to choose the frequency manually
    

df = pd.read_csv('1012232.txt'); df
# spacing = (((df['time'].iloc[-1]-df['time'].iloc[0])*10**-6))
# rate = 1/((df['time'].iloc[-1]-df['time'].iloc[1])/len(df['time'])*10**-6)
# print(rate)
N=len(df['raw data'])
# rate = 36488
rate = 90000/(5.248-2.030)


f, t, Sxx = spectrogram(df['raw data'],rate, nfft=600)
plt.pcolormesh(t, f, Sxx, shading='gouraud')
plt.ylabel('Frequency [Hz]')
plt.xlabel('Time [sec]')
plt.show()
#y = Sxx[min(range(len(f)), key=lambda i: abs(f[i]-440))]
y = Sxx[Sxx.sum(axis=1).argmax()]
print(Sxx.sum(axis=1).argmax())


def line(x, b, m = 0):
    return m*x + b

def exponential(x, a, k, p, o = 0):
    return a*np.exp((x-p)*k) + o

def func(xarray,b,k,p):
    returnarray = []
    for x in xarray:
        if x < p:
            returnarray.append(line(x,b))
        else:
            returnarray.append(exponential(x,b,k,p))
    return returnarray
    
popt, pcov = curve_fit(func, t, y, p0=[50,0,0], 
                       bounds = ((40,-np.inf, 0),(60, 0, 3)), 
                       maxfev=100000)

plt.plot(t, func(t, *popt))
print(popt)
plt.plot(t, y)
plt.show()
print('-3dB time is ', np.log(0.5)/popt[1], 's for a frequency of ', f[Sxx.sum(axis=1).argmax()], 'Hz')


# vf = fft(np.array((df['voltage'])))
# xf = fftfreq(N, spacing)[:N//2]
# plt.plot(xf, np.abs(vf[:N//2])  )

# plt.plot(df['time'], df['voltage'])
# plt.show()
    



