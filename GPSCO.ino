// A minimum solution to manually calibrating a bare OCXO module 
// with GPS using the 1 pps signal
//
// The 1 pps signal is used to trigger an interrupt
// and start counting the OCXO (TTL) pulses using the 16-bit timer1
// until a definable number of pps-signals has passed.
// Each overflow of timer1 creates an interrupt and is counted,
// and - together with the last timer1 value - is used
// to calculate the total number of OCXO pulses.

// uC: Atmega328P / Arduino Uno R3
// External hardware: 74HC390 or 74HCT390 (counter) 
//
// Christoph Schwaerzler, OE1CGS
// V1.1 29.11.2023

#include <string.h>
#include <ctype.h>
#include <avr/interrupt.h>  
#include <avr/io.h>


// Parameters
#define tpause                     1 // [>=1] Integer number of pps-signals (i.e. seconds) between measurements (and prior to first measurement)
#define tmeasure                 200 // [>=1] Integer number of pps-signals (i.e. seconds) of one measurement
#define ppsPin                     2 // Pin D2 (INT0) for 1pps signal from GPS 
#define OCXOPin                    5 // Pin D5 (T1)  for 10 MHz OCXO (TTL)
#define LEDPin                    13 // Pin D13 for (built in) LED, alternating with 1 pps signal
#define divider                    2 // Factor of the external divider

// Variables
unsigned int  nmeasure          = 0; // Number of current measurement 
unsigned long count             = 0; // Count of overflows of 16 bit counter
unsigned int low_count          = 0; // Value of the counter in timer1
volatile unsigned int ppscount  = 0; // Count of 1pps pulses


void setup()
{
  pinMode(ppsPin, INPUT_PULLUP);
  pinMode(OCXOPin, INPUT_PULLUP);
  pinMode(LEDPin, OUTPUT);

  //Set up Timer1 as a frequency counter - input at pin D9
  TCCR1B = 0;                        //No clock source (Timer/Counter stopped)
  TCCR1A = 0;                        //Normal port operation, OC1A/OC1B disconnected, Normal mode (counter)
  TCNT1  = 0;                        //Set counter value to zero
  TIFR1  = 1;                        //Reset overflow
  TIMSK1 = 1;                        //Turn on overflow flag

  // Set Arduino D2 for external interrupt input on the rising edge of GPS 1PPS
  attachInterrupt(0, PPSinterrupt, RISING);  
  
  // Initialize serial port and print headers and measurement info on the serial port
  Serial.begin(9600);                // Initialize serial port
  while(!Serial){;}                  // Wait for serial port to connect
  Serial.print("#");
  Serial.print("\t\t");              // Move next output two tabulators to the right
  Serial.print("Counts");
  Serial.print("\t\t");
  Serial.print("tmeasure=");
  Serial.println(tmeasure);
}

// Timer1 overflow interrupt vector increments the counter
ISR(TIMER1_OVF_vect) 
{
  count++;                           //Increment the overflows counter
  TIFR1 = (1<<TOV1);                 //Clear overflow flag by shifting left 
}


// Interrupt routine for counting the 10 MHx OCXO signal
// Called every second by GPS 1PPS on Arduino Uno D2

void PPSinterrupt()
{
  if (ppscount == tpause)
  {
    TCNT1  = 0;                      // Set counter to zero
    TCCR1B = 7;                      // Enable Timer1 to count on leading edge of T1 (Pin D5)
  }
  if (ppscount == tpause + tmeasure)
  {
    TCCR1B    = 0;                   // Stops counting
    low_count = TCNT1;               // Stores last value of counter in Timer1
    ppscount  = 0;                   // Reset the pps counter for the next measurement
    nmeasure++;                      // Increase the number of measurements
    count = (count << 16) + low_count;
    // Print current and average data
    Serial.print(nmeasure);          // Print the number of the current measurement
    Serial.print("\t\t");
    Serial.println(count);             // Print the total number of cycles measured during the last measurement time 'tmeasure'
    count = 0;
  }
  ppscount++;                                            // Increment the pps counter
  digitalWrite(LEDPin, !digitalRead(LEDPin));            // LED blinks with pps signal
  loop();
}

void loop()
{ 

}

