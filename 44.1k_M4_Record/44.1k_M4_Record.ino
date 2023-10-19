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

void setup() {
  // put your setup code here, to run once:

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

void loop() {
  // put your main code here, to run repeatedly:
  if (samp_full == 1) {
    Serial.println(aud_sampl[0]);
  }
  
}

/* FFT Code
 *  Code only works for sample numbers (DATA_SIZE) between 2 and 2048, in powers of two
 *  For a good graph on the Serial plotter, 512 seems to be optimal (bc serial plotter only keeps 
 *  last 500 points, if you set data_size to 1024 it cuts off the low frequency data)
 *   while(!samp_full);
   int32_t avg = 0;
   int16_t signal[DATA_SIZE - 1];
   for (int j=0;j<DATA_SIZE;j++) {
      avg += aud_sampl[j];
      signal[j] = aud_sampl[j];
      //Serial.println(aud_sampl[j]);
   }

   //remove DC offset and gain up to 16 bits
  avg = avg/DATA_SIZE;
  for(int i=0; i<DATA_SIZE; i++) signal[i] = (signal[i] - avg) * 16;
    
     //run the FFT
  ZeroFFT(signal, DATA_SIZE);

  //data is only meaningful up to sample rate/2, discard the other half
  for(int i=0; i<DATA_SIZE/2; i++){
    
    //print the frequency
    Serial.print(FFT_BIN(i, SAMPLE_RATE, DATA_SIZE) / 1000);
    Serial.print(" kHz: ");

    //print the corresponding FFT output
    Serial.println(signal[i]);
  }
 *  
 *  
 *  
 *  
 */
