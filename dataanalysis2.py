# -*- coding: utf-8 -*-
"""
Created on Thu Nov  2 15:55:46 2023

@author: jinen
"""

import numpy as np
from scipy.optimize import curve_fit
from scipy.optimize import differential_evolution
import warnings
import matplotlib.pyplot as plt
import pandas as pd
from scipy.signal import spectrogram
from scipy.fft import fft, fftfreq
def plot(filepath,i,idx,h):


    #Fq = 440 #this is if you dont want to choose the frequency manually
        
    df = pd.read_csv(filepath); df
    rate = df.iloc[-1]['raw data']
    df.drop(df.index[-1], inplace=True)
    print(rate)
    # spacing = (((df['time'].iloc[-1]-df['time'].iloc[0])*10**-6))
    # rate = 1/((df['time'].iloc[-1]-df['time'].iloc[1])/len(df['time'])*10**-6)
    # print(rate)
   
    # rate = 36488
    
    
    f, t, Sxx = spectrogram(df['raw data'],rate, nfft=600)
    plt.figure(h)
    
    plt.subplot(2,2,1)

    plt.pcolormesh(t, f, Sxx, shading='gouraud')
    plt.ylabel('Frequency [Hz]')
    plt.xlabel('Time [sec]')
    plt.ylim(0,1500)
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
    plt.title('Device '+str(i)+' Runs '+str(idx)) 
    print('Device '+str(i)+' Runs '+str(idx)) 
    plt.subplot(2,2,2)

    plt.plot(t, func(t, *popt), label = "{:0.2f}e^{:0.2f}x".format(popt[0],popt[1]))
    plt.plot(t, y, label = ' {:0.2f} Hz'.format(f[Sxx.sum(axis=1).argmax()]))
    plt.title("Ringdown of {:0.2f} Hz (Log scale)".format(f[Sxx.sum(axis=1).argmax()]))
    plt.ylabel("Intensity")
    plt.xlabel("Time (s)")
    plt.yscale("log")
    plt.legend()
    plt.show()
    print('RT60 time is {:0.2f} s for a frequency of {:0.2f} Hz'.format(np.log(0.001*popt[0])/popt[1], f[Sxx.sum(axis=1).argmax()]))
    print('Decay time, \u03C4 = {:0.2f} s'.format(-1/popt[1]))
    
    
    # vf = fft(np.array((df['voltage'])))
    # xf = fftfreq(N, spacing)[:N//2]
    # plt.plot(xf, np.abs(vf[:N//2])  )
    plt.subplot(2,2,3)
    plt.title("Raw Data")
    plt.xlabel("Time (s)")
    plt.ylabel("Digital Value")
    plt.plot(np.linspace(0,2.05, num=len(df['raw data'])), df['raw data'])
    plt.show()
    
    plt.subplot(2,2,4)

    plt.plot(t, func(t, *popt), label = "{:0.2f}e^{:0.2f}x".format(popt[0],popt[1]))
    plt.plot(t, y, label = ' {:0.2f} Hz'.format(f[Sxx.sum(axis=1).argmax()]))
    plt.title("Ringdown of {:0.2f} Hz (Log Scale) ".format(f[Sxx.sum(axis=1).argmax()]))
    plt.title("RT60 time is {:0.2f} s for a frequency of {:0.2f} Hz,Decay time, \u03C4 = {:0.2f} s ".format(np.log(0.001*popt[0])/popt[1], f[Sxx.sum(axis=1).argmax()],-1/popt[1]))
    plt.ylabel("Intensity")
    plt.xlabel("Time (s)")
    plt.legend()
    plt.show()





import os
import pandas as pd
import numpy as np
from datetime import datetime
def read_files_in_directory(directory_path):
    dfs = {}  # A dictionary to store DataFrames
    idx = 1  # Index for naming
    h=1
    for i in range(1,4):
        dfs[i]={}
        idx=1
        
        for filename in os.listdir(directory_path):
            
            if filename.endswith(".TXT") and filename.startswith("D"+str(i)):
                
                print(filename)
                filepath = os.path.join(directory_path, filename)  # Get full path
                dfs[i][f'trial{idx}'] = pd.read_csv(filepath)
                
                plot(filepath,i,idx,h)
                now = datetime.now()
                date_time1 = now.strftime("%m-%d-%Y")
                date_time = now.strftime("%m-%d-%Y%H%M%S")
                save_direct = 'C:\\Users\\jinen\\OneDrive - University of Illinois - Urbana\\Desktop\\New Folder (7)\\'+date_time1
                if h==1 and not os.path.exists(save_direct):
                    os.mkdir(date_time1) 
               
                plt.savefig(os.path.join(save_direct,date_time+'Device '+str(i)+' Runs '+str(idx)))
                idx += 1
         
                h +=1 
    return dfs

directory_path = os.getcwd()
dataframes = read_files_in_directory(directory_path)


combined_voltage = pd.DataFrame()
a={}
for i in range(1,4):
  
    a[i]={}
    combined_voltage = pd.DataFrame()

    for trial_name, df in dataframes[i].items():
     
        if 'raw data' in df.columns:
            
            
            combined_voltage[trial_name] = df['raw data']
            a[i]=combined_voltage
b=np.array(a[1])
print(b)









