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
    
df = pd.read_csv('1024231.txt'); df
# spacing = (((df['time'].iloc[-1]-df['time'].iloc[0])*10**-6))
# rate = 1/((df['time'].iloc[-1]-df['time'].iloc[1])/len(df['time'])*10**-6)
# print(rate)
N=len(df['raw data'])
# rate = 36488
#rate = 90000/(5.248-2.030)
rate = 70000/2.05

f, t, Sxx = spectrogram(df['raw data'],rate, nfft=600)
plt.figure(1)
plt.pcolormesh(t, f, Sxx, shading='gouraud')
plt.title("Spectrogram")
plt.ylabel('Frequency [Hz]')
plt.xlabel('Time [sec]')
plt.show()
#y = Sxx[min(range(len(f)), key=lambda i: abs(f[i]-Fq))]
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
    
popt, pcov = curve_fit(func, t, y, p0=[0,0,1.5], 
                       bounds = ((0,-np.inf, 0),(100, 0, 3)), 
                       maxfev=100000)

plt.figure(2)
plt.plot(t, func(t, *popt), label = "{:0.2f}e^{:0.2f}".format(popt[0],popt[1]))
plt.plot(t, y, label = ' {:0.2f} Hz'.format(f[Sxx.sum(axis=1).argmax()]))
plt.title("Ringdown of {:0.2f} Hz ".format(f[Sxx.sum(axis=1).argmax()]))
plt.ylabel("Intensity")
plt.xlabel("Time (s)")
plt.legend()
plt.show()
print('RT60 time is {:0.2f} s for a frequency of {:0.2f} Hz'.format(np.log(0.001)/popt[1], f[Sxx.sum(axis=1).argmax()]))
print('Decay time, \u03C4 = {:0.2f} s'.format(-1/popt[1]))

# vf = fft(np.array((df['voltage'])))
# xf = fftfreq(N, spacing)[:N//2]
# plt.plot(xf, np.abs(vf[:N//2])  )

plt.figure(3)
plt.title("Raw Data")
plt.xlabel("Time (s)")
plt.ylabel("Digital Value")
plt.plot(np.linspace(0,2.05, num=len(df['raw data'])), df['raw data'])
plt.show()
    



