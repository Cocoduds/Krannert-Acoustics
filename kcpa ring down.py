# -*- coding: utf-8 -*-
"""
Created on Thu Sep  7 16:18:23 2023

@author: dudle
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
y = Sxx[min(range(len(f)), key=lambda i: abs(f[i]-440))]
plt.plot(t, y)
plt.show()


def func(xArray, breakpoint1, breakpoint2, slopeA, offsetA, slopeB, offsetB, slopeC, offsetC):
    returnArray = []
    for x in xArray:
        if x < breakpoint1:
            returnArray.append(slopeA * x + offsetA)
            
        if (breakpoint1 < x < breakpoint2):
            returnArray.append(slopeB * x + offsetB)
            
        else:
            returnArray.append(slopeC * x + offsetC)
    return returnArray


# function for genetic algorithm to minimize (sum of squared error)
def sumOfSquaredError(parameterTuple):
    warnings.filterwarnings("ignore") # do not print warnings by genetic algorithm
    val = func(t, *parameterTuple)
    return np.sum((y - val) ** 2.0)


def generate_Initial_Parameters():
    # min and max used for bounds
    maxX = max(t)
    minX = min(t)
    maxY = max(y)
    minY = min(y)
    slope = 10.0 * (maxY - minY) / (maxX - minX) # times 10 for safety margin

    parameterBounds = []
    parameterBounds.append([minX, maxX]) # search bounds for breakpoint
    parameterBounds.append([minX, maxX]) # search bounds for breakpoint 2
    parameterBounds.append([-slope, slope]) # search bounds for slopeA
    parameterBounds.append([minY, maxY]) # search bounds for offsetA
    parameterBounds.append([-slope, slope]) # search bounds for slopeB
    parameterBounds.append([minY, maxY]) # search bounds for offsetB
    parameterBounds.append([-slope, slope]) # search bounds for slopeC
    parameterBounds.append([minY, maxY]) # search bounds for offsetC


    result = differential_evolution(sumOfSquaredError, parameterBounds, seed=3)
    return result.x

# by default, differential_evolution completes by calling curve_fit() using parameter bounds
geneticParameters = generate_Initial_Parameters()

# call curve_fit without passing bounds from genetic algorithm
fittedParameters, pcov = curve_fit(func, t, y, geneticParameters)
print('Parameters:', fittedParameters)
print()

modelPredictions = func(t, *fittedParameters) 

absError = modelPredictions - y

SE = np.square(absError) # squared errors
MSE = np.mean(SE) # mean squared errors
RMSE = np.sqrt(MSE) # Root Mean Squared Error, RMSE
Rsquared = 1.0 - (np.var(absError) / np.var(y))

print()
print('RMSE:', RMSE)
print('R-squared:', Rsquared)

print()


##########################################################
# graphics output section
def ModelAndScatterPlot(graphWidth, graphHeight):
    f = plt.figure(figsize=(graphWidth/100.0, graphHeight/100.0), dpi=100)
    axes = f.add_subplot(111)

    # first the raw data as a scatter plot
    axes.plot(t, y,  'D')

    # create data for the fitted equation plot
    xModel = np.linspace(min(t), max(t))
    yModel = func(xModel, *fittedParameters)

    # now the model as a line plot
    axes.plot(xModel, yModel)

    axes.set_xlabel('X Data') # X axis data label
    axes.set_ylabel('Y Data') # Y axis data label

    plt.show()
    plt.close('all') # clean up after using pyplot

graphWidth = 800
graphHeight = 600
ModelAndScatterPlot(graphWidth, graphHeight)



# vf = fft(np.array((df['voltage'])))
# xf = fftfreq(N, spacing)[:N//2]
# plt.plot(xf, np.abs(vf[:N//2])  )

# plt.plot(df['time'], df['voltage'])
# plt.show()
    



