# -*- coding: utf-8 -*-
"""
Created on Thu Sep  7 16:18:23 2023

@author: colton, neng, christian, josh

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
import os


def read_files_in_directory(directory_path):
    dfs = {}  # A dictionary to store DataFrames
    idx = 1  # Index for naming
    for i in range(1,4):
        dfs[i]={}
        idx=1
        for filename in os.listdir(directory_path):
            if filename.endswith(".TXT") and filename.startswith("d"+str(i)):
                filepath = os.path.join(directory_path, filename)  # Get full path
                dfs[i][f'trial{idx}'] = pd.read_csv(filepath)
                idx += 1
    return dfs


directory_path = os.getcwd()
dataframes = read_files_in_directory(directory_path)

plt.close('all')

Fq = 370 #this is if you dont want to choose the max Fq
    
df = pd.read_csv('D11.txt'); df
# spacing = (((df['time'].iloc[-1]-df['time'].iloc[0])*10**-6))
# rate = 1/((df['time'].iloc[-1]-df['time'].iloc[1])/len(df['time'])*10**-6)
# print(rate)
N=len(df['raw data'])
# rate = 37700
#rate = 90000/(5.248-2.030)
# rate = 70000/2.05
rate = df.iloc[-1]['raw data']
df.drop(df.tail(1).index, inplace = True)

f, t, Sxx = spectrogram(df['raw data'],rate, nfft=600)
plt.figure(1)
plt.pcolormesh(t, f, Sxx, shading='gouraud')
plt.title("Spectrogram")
plt.ylabel('Frequency [Hz]')
plt.xlabel('Time [sec]')
plt.show()
# y = Sxx[min(range(len(f)), key=lambda i: abs(f[i]-Fq))] #for picking Fq
y = Sxx[Sxx.sum(axis=1).argmax()]
# print(Sxx.sum(axis=1).argmax())


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
plt.plot(t, func(t, *popt), label = "{:0.2f}e^{:0.2f}x".format(popt[0],popt[1]))
plt.plot(t, y, label = ' {:0.2f} Hz'.format(f[Sxx.sum(axis=1).argmax()]))
plt.title("Ringdown of {:0.2f} Hz ".format(f[Sxx.sum(axis=1).argmax()]))
plt.ylabel("Intensity")
plt.xlabel("Time (s)")
plt.legend()
plt.show()
print('RT60 time is {:0.2f} +- {:0.2f} s for a frequency of {:0.2f} Hz'.format(np.log(0.001)/popt[1], (np.log(0.001)/popt[1])*np.sqrt(pcov[1,1])/-popt[1], f[Sxx.sum(axis=1).argmax()])) #possible error np.sqrt((pcov[0,0]/popt[0]**2)**2+(pcov[1,1]/popt[1])**2)
print('Decay time, \u03C4 = {:0.2f} +- {:0.2f} s'.format(-1/popt[1], (1/popt[1])*np.sqrt(pcov[1,1])/popt[1]))

# vf = fft(np.array((df['voltage'])))
# xf = fftfreq(N, spacing)[:N//2]
# plt.plot(xf, np.abs(vf[:N//2])  )

plt.figure(3)
plt.title("Raw Data")
plt.xlabel("Time (s)")
plt.ylabel("Digital Value")
plt.plot(np.linspace(0,2.05, num=len(df['raw data'])), df['raw data'])
plt.show()
    



