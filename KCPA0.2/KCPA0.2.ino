
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
  Serial.begin(57600);
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
int startTime = micros();
void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V): 
  float voltage = sensorValue * (3.0 / 1023.0);
  // print out the value you read:

  if (micros() < startTime + 10000000){
    Serial.println(voltage);
    myFile = SD.open("data.txt", FILE_WRITE);
    if(myFile){
      myFile.print(voltage);
      myFile.print(",");
      myFile.print(sensorValue);
      myFile.print(",");
      myFile.print(micros());
      myFile.println("");
      myFile.close(); 
    } else {
      // if the file didn't open, print an error:
      Serial.println("error opening test.txt");
    }
  }
  else{
//    Serial.print("Off: ");
    Serial.println("done");
  }
}
