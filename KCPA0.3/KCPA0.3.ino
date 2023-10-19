
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <SD.h>

// set up variables using the SD utility library functions:
File myFile;
// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// MKRZero SD: SDCARD_SS_PIN
const int chipSelect = 4;


void setup() {
  Serial.begin(1000000);
  while (!Serial){
    ;
  }
  Serial.println(F("Mic test"));

  Serial.print("\nInitializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  SD.remove("data.txt");
  myFile = SD.open("data.txt", FILE_WRITE);
  myFile.print("voltage,time");
  myFile.println("");
  myFile.close();  
  Serial.println("initialization done.");
}
int i = 0;
const uint16_t size = 2000;
int startTime = micros();
float voltageBuffer[size];
int timeBuffer[size];
void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V): 
  float voltage = sensorValue * (3.0 / 1023.0);
  // print out the value you read:

  if (i<size){
//    Serial.println(voltage);
    voltageBuffer[i] = voltage;
    timeBuffer[i] = micros();
    i++;
  }
  if (i==size){
    myFile = SD.open("data.txt", FILE_WRITE);
    if(myFile){
      for (int j = 0; j<size; j++){
        myFile.print(voltageBuffer[j]);
        myFile.print(",");
        myFile.print(timeBuffer[j]);
        myFile.println("");
      }
      myFile.close(); 
    } else {
      // if the file didn't open, print an error:
      Serial.println("error opening test.txt");
    }
    Serial.println("done");
    i++;
  }
}
