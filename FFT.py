# -*- coding: utf-8 -*-
"""
Created on Thu Oct  5 14:20:11 2023

@author: jjake
"""

import numpy as np 
import scipy.signal as signal
import scipy.fft as fft 
import matplotlib.pyplot as plt 
import pandas as pd
from datetime import datetime

#Read the data from text file
df = pd.read_csv('1024232.txt')
sin_wave = df['raw data'].to_numpy()
#sin_wave = data[9:]
sample_rate= 45000

#Fourier Transform

fft_result = fft.fft(sin_wave) 
fft_result_less_zero = np.delete(np.abs(fft_result), 0)
#Frequency axis for the FFT result
freq_axis = fft.fftfreq(len(fft_result), 1 / sample_rate) 
freq_axis_less_zero = np.delete(freq_axis, 0)
#Plot the FFT result
plt.figure() 
plt.plot(freq_axis_less_zero, fft_result_less_zero) 
plt.title('FFT Result') 
plt.xlabel('Frequency (Hz)') 
plt.ylabel('Magnitude') 
plt.xlim(0,10000)
plt.ylim(0, np.max(fft_result_less_zero))
plt.show()
#print(signal.find_peaks(fft_result_less_zero)

"""
#check sample rate
original_frequency = 440
peak_loc = np.argmax(fft_result_less_zero)
rate = peak_loc / original_frequency
print("rate ratio= ", rate)
"""

"""
#save file with date and time
now = datetime.now()
date_time = now.strftime("%m-%d-%Y %H-%M-%S")
print(date_time)
plt.savefig(date_time + ".png") 
"""

#my_path = os.path.abspath(C:\Users\jjake\Documents\UIUC\Krannert Project\Data) # Figures out the absolute path for you in case your working directory moves around.
#my_file = 'graph.png'
#plt.savefig(os.path.join(my_path, my_file))