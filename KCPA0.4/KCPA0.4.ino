#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <SD.h>
#include "Adafruit_FRAM_SPI.h"

int analogPin = A0;

// set up variables using the SD utility library functions:
File myFile;
// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// MKRZero SD: SDCARD_SS_PIN
const int chipSelect = 11; //4 for adalogger

int pin = 12;
int pinOut = 13;

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
const int size = 90000;
int startTime = micros();
int r = 0;
uint16_t sensorValue = 0;
uint16_t rawBuffer[size];

void loop() {
  if(r==0){
    myFile = SD.open("data.txt", FILE_WRITE);

    int sensorValue = analogRead(A0);
    if (i<size){
      rawBuffer[i] = sensorValue;
      i++;


  }

  }
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);

  if (i<size){
    rawBuffer[i] = sensorValue;
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
