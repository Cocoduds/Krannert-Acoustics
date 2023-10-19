
  #include <Wire.h>
  #include <SPI.h>
  #include <Adafruit_Sensor.h>
  #include <SD.h>
  #include "Adafruit_FRAM_SPI.h"

  /** Code to sample the Feather M4's ADC0 at 44.1 kHz, 12-bits per sample (It's *almost* Red Book audio :P Could get there w/averaging but not worth it)
 *  May include code to sample both ADCs at once, as well as variable sampling rate
 *  Uses TimerInterrupt_Generic library to generate precise interrupts using hardware timing
 *  Feature TODO: dumping audio to SD card, dumping audio thru serial to computer, add IR remote control, write to internal Flash
 *  Implemented: Rough frequency analysis code (commented out)
 *  Sample 4 mics at a time ?? Use software timers probably
 *  Idea: stagger timers, measure mics 1+3 then 2+4
 *  Need to time things right!
 *  For Physics 398 DLP, Spring 2021
 *  Author: Ivan Velkovsky, v0.2, 2021-02-15
 */

#include <TimerInterrupt_Generic.h>
#include <ISR_Timer_Generic.h>
#include <Adafruit_ZeroFFT.h>
#include <arm_common_tables.h>


/* Sets up the ADC. Uses prescaling factor of 128 for good stability,
 *  since to support 44.1kHz sampling on a 120 MHz board the max prescaling
 *  is ~200 (13 ADC clock cycles needed for 12 bit measurement)
 *  Adjust parameters here to change resolution & averaging.
 *  TODO: add case statement to use this code to initialize ADC1 if needed
 */
void ADC_init() {
  ADC0->CTRLA.bit.ENABLE = 0;                     // Disable ADC
  while( ADC0->STATUS.bit.ADCBUSY == 1 );        // Wait for synchronization
  ADC0->CTRLA.reg = ADC_CTRLA_PRESCALER_DIV128 ;   // Divide Clock ADC GCLK by 128 (120 MHz / 128 = 937.5 kHz)
  ADC0->CTRLB.reg =  ADC_CTRLB_RESSEL_12BIT;         // Set ADC resolution to 12 bits
  
  ADC0->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_1 |   // Take a single sample per measurement
                     ADC_AVGCTRL_ADJRES(0x00ul); //  No bit shifting of the sample
                     
  ADC0->SAMPCTRL.reg = 0x00;                        // Set max Sampling Time Length to half divided ADC clock pulse (5.33us)
  ADC0->INPUTCTRL.bit.MUXPOS = 0x00;
  //ADC0->CTRLB.bit.FREERUN = 1;
  ADC0->CTRLA.bit.ENABLE = 1;                     // Enable ADC
   ADC0->SWTRIG.bit.START = 1;
  while( ADC0->STATUS.bit.ADCBUSY == 1 );        // Wait for synchronization

}


/* Setup code for the timer
 * To change sampling frequency, make sure that the ADC prescaler
 * and sampling time/averaging parameters are compatible with your
 * new sampling frequency.
 */

SAMDTimer ITimer0(TIMER_TC3);
#define TIMER0_INTERVAL_US 22.68 // 1/44.1kHz 
#define DATA_SIZE 512
#define SAMPLE_RATE 44100

volatile uint16_t aud_sampl[DATA_SIZE];
volatile int sam_curr = 0;
volatile bool samp_full = 0;
void TimerHandler0(void) {
  //TODO: Sampling
  if (!samp_full) {
       ADC0->SWTRIG.bit.START = 1;
  while( ADC0->STATUS.bit.ADCBUSY == 1 ); 
    volatile uint16_t val = ADC0->RESULT.reg;
    aud_sampl[sam_curr] = val;
    sam_curr++;
    if (sam_curr == DATA_SIZE) {
      samp_full = 1;
    }
    }
}

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

/* Example code for the Adafruit SPI FRAM breakout */
uint8_t FRAM_CS = 10;
Adafruit_FRAM_SPI fram = Adafruit_FRAM_SPI(FRAM_CS);  // use hardware SPI

uint16_t          addr = 0;

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


  //FRAM Setup
  if (fram.begin()) {
    Serial.println("Found SPI FRAM");
  } else {
    Serial.println("No SPI FRAM found ... check your connections\r\n");
    while (1);
  }

  pinMode(pin, INPUT);  
  pinMode(pinOut, OUTPUT);

  Serial.println("initialization done.");

  ADC_init();
  Serial.begin(115200);
  while (!Serial); // Wait for the serial to be ready
  if (ITimer0.attachInterruptInterval(TIMER0_INTERVAL_US, TimerHandler0)) {
    //Serial.println("Starting  ITimer0 OK, millis() = " + String(millis()));
  }
  else {
    //Serial.println("Can't set ITimer0. Select another freq. or timer");
  }

   //Serial.println("Setup done");


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
    //digitalWrite(pinOut, HIGH);
    while(digitalRead(pin) == LOW){
      ;
    }
    //digitalWrite(pinOut, LOW);
    r++;
    startTime = micros();
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
