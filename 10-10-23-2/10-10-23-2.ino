#include "Adafruit_FRAM_SPI.h"
#include "arduinoFFT.h"

arduinoFFT FFT;

uint8_t FRAM_CS = 10;
Adafruit_FRAM_SPI fram = Adafruit_FRAM_SPI(FRAM_CS);  // use hardware SPI

uint16_t          addr = 0;

/*
These values can be changed in order to evaluate the functions
*/
const int samples = 90000; //This value MUST ALWAYS be a power of 2
const double signalFrequency = 1000;
double samplingFrequency = 35000;
const uint8_t amplitude = 100;
double startTime = 0;
int r = 0;
/*
These are the input and output vectors
Input vectors receive computed results from FFT
*/
uint16_t vReal[samples];

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

void setup()
{
  Serial.begin(115200);
  while(!Serial);

  //FRAM Setup
  if (fram.begin()) {
    Serial.println("Found SPI FRAM");
  } else {
    Serial.println("No SPI FRAM found ... check your connections\r\n");
    while (1);
  }

  Serial.println("Ready");
}

void loop()
{
  if (r==0){
    startTime = micros();
    Serial.println("Recording");
    for (int i = 0; i < samples; i++)
    {
      vReal[i] = uint16_t(analogRead(A0));/* Build data displaced on the Y axis to include only positive values*/
    }
    r=1;
  }
  //Serial.println(((micros()-startTime)*0.000001));
  startTime = micros();
  Serial.println("Playing");
  for (int i = 0; i < samples; i++)
  {
    analogWrite(A1, vReal[i]);
    delayMicroseconds(25);
  }
  //Serial.println(((micros()-startTime)*0.000001));
}

void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType)
{
  for (uint16_t i = 0; i < bufferSize; i++)
  {
    double abscissa;
    /* Print abscissa value */
    switch (scaleType)
    {
      case SCL_INDEX:
        abscissa = (i * 1.0);
	break;
      case SCL_TIME:
        abscissa = ((i * 1.0) / samplingFrequency);
	break;
      case SCL_FREQUENCY:
        abscissa = ((i * 1.0 * samplingFrequency) / samples);
	break;
    }
    Serial.print(abscissa, 6);
    if(scaleType==SCL_FREQUENCY)
      Serial.print("Hz");
    Serial.print(" ");
    Serial.println(vData[i], 4);
  }
  Serial.println();
}

