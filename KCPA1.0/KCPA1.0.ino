#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <SD.h>
#include "Adafruit_FRAM_SPI.h"

#include "arduinoFFT.h"
arduinoFFT FFT;

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

int analogPin = A0;

// set up variables using the SD utility library functions:
File myFile;
// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// MKRZero SD: SDCARD_SS_PIN
const int chipSelect = 11; //4 for adalogger


void setup() {
  //analogReadResolution(12);
  Serial.begin(1000000);
  while (!Serial){
    ;
  }
  Serial.println(F("Mic test"));

  //SD card setup
  Serial.print("\nInitializing SD card...");
  if (!SD.begin(11)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  SD.remove("data.txt");
  myFile = SD.open("data.txt", FILE_WRITE);
  myFile.print("raw data");
  myFile.println("");
  myFile.close();  

  Serial.println("initialization done.");

}
int i = 0;
const int size = 70000; /* can increase to like 86000 if you reduce waiting buffer size to 1024*/
int startTime = micros();
int r = 0;
uint16_t sensorValue = 0;
uint16_t rawBuffer[size];
double samplingFrequency = 35000;
const int waitsize = 2048; /* must be a multiple of 2 for fft*/
double waitBuffer[waitsize];
double vImag[waitsize];
double x2 = 0;
double x3 = 0;
double x4 = 0;

void loop() {
  if(r==0){
    startTime = micros();
    while (i < waitsize){
      waitBuffer[i] = uint8_t(analogRead(A0));
      vImag[i] = 0.0; //need to specify that these are 0 
      i++;
    }
    if (i == waitsize){
      i = 0;
      samplingFrequency = waitsize / ((micros()-startTime)*0.000001);
      FFT = arduinoFFT(waitBuffer, vImag, waitsize, samplingFrequency);
      FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);	/* Weigh data */
      FFT.Compute(FFT_FORWARD); /* Compute FFT */
      FFT.ComplexToMagnitude(); /* Compute magnitudes */  
      double x = FFT.MajorPeak();
      Serial.println(x);
      if ((abs(x-x2)<2 && abs(x-x3)<2)||x>1100){ /* playing around with the window. can also use x4 */
        r = 1;
        Serial.println("starting");
        myFile = SD.open("data.txt", FILE_WRITE);
      }
      else{
        x4 = x3;
        x3 = x2;
        x2 = x;
      }
    }
  }

  if (i<size){
    rawBuffer[i] = analogRead(A0);
    i++;
  }

  if (i==size){
    Serial.println((micros() - startTime)*0.000001);
    if(myFile){
      for (int j = 0; j<size; j++){
        myFile.println(rawBuffer[j]);
      }
    } else {
      // if the file didn't open, print an error:
      Serial.println("error opening data file");
    }
    Serial.println("done");
    i = 0;
    r++;
  }
  if(r==2){
    myFile.close(); 
    delay(2000000);
  }
}
