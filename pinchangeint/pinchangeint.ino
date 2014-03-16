/***************************************************************************** 
 * Sketch:  PC_IRQ_SINGLE.pde
 * Author:  A. Kriwanek: http://www.kriwanek.de/arduino/komponenten.html
 * Version: 1.0  26.06.2011/14:35
 *
 * This sketch is waiting for a pin change interrupt on an Arduino pin. If an interrupt
 * occurs, the interrupt service routine is called. The routine increments a counter-
 * The main program sends the counter value over the serial port. The interrupt
 * service routine debounces the input signal (e.g. for a switch contact). 
 *
 * This sketch is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 *****************************************************************************/
//-----------------------------------------------------------------------------

// Define values for the interrupt counter:
volatile int counter1    = 0;        // Counter incremented by pin change interrupt
volatile int bounceTime  = 20;       // Switch bouncing time in milliseconds
volatile unsigned long IRQ1PrevTime; // Last time in milliseconds IRQ1 arrived
volatile int IRQ1PrevVal   = 0;      // Contact level last IRQ1
volatile int irqFlag1 = 0;           // 1=display counter after IRQ1; 0=do nothing

volatile int counter2    = 0;        // Counter incremented by pin change interrupt
volatile unsigned long IRQ2PrevTime; // Last time in milliseconds IRQ1 arrived
volatile int IRQ2PrevVal   = 0;      // Contact level last IRQ1
volatile int irqFlag2 = 0;           // 1=display counter after IRQ1; 0=do nothing

// Setup and Main:
void setup(){
  Serial.begin(57600);                // Initialize serial interface with 57600 Baud
  Serial.write("Waiting for an interrupt...\n");
  // Make Arduino Pin 7 (PCINT23/PD7) an input and set pull up resistor:
  pinMode(4, INPUT);
  digitalWrite(4, HIGH);
 pinMode(14, INPUT);
  digitalWrite(14, HIGH);
  // This is ATMEGA368 specific, see page 75 of long datasheet
  // PCICR: Pin Change Interrupt Control Register - enables interrupt vectors
  // Bit 2 = enable PC vector 2 (PCINT23..16)
  // Bit 1 = enable PC vector 1 (PCINT14..8)
  // Bit 0 = enable PC vector 0 (PCINT7..0)
  PCICR |= (1 << PCIE2);             // Set port bit in CICR
  PCICR |= (1 << PCIE1);             // Set port bit in CICR
  // Pin change mask registers decide which pins are enabled as triggers:
  PCMSK2 |= (1<<PCINT20);            // Set pin interrupt for digital input
  PCMSK1 |= (1<<PCINT8);            // Set pin interrupt for digital input
  IRQ1PrevTime=millis();             // Hold actual time
  IRQ2PrevTime=millis();             // Hold actual time
  interrupts();                      // Enable interrupts
}

void loop()
{
  // Place your main loop commands here (e.g. output to LCD)
  if (irqFlag1==1)                   // Flag was set by IRQ1 routine
  {
    Serial.write("IRQ-A gruen rising edge, Counter1 = ");
    Serial.println(counter1);
    irqFlag1=0;                      // Reset IRQ1 flag
  }
  if (irqFlag2==1)                   // Flag was set by IRQ1 routine
  {
    Serial.write("IRQ-B blau rising edge, Counter2 = ");
    Serial.println(counter2);
    irqFlag2=0;                      // Reset IRQ1 flag
  }
}

//-----------------------------------------------------------------------------
// Subs and Functions:

ISR(PCINT2_vect)
{
  // You have to write your own interrupt handler. Don't change the name!
  // This code will be called anytime when PCINT23 switches high to low, 
  // or low to high.
  byte PVal;                                   // Port value (8 Bits)
  byte IRQ1ActVal;                             // Actual IRQ1 value
  long unsigned IRQ1ActTime;

  PVal = PIND;                                 // Read port D (8 bit)
  IRQ1ActVal = PVal & (1<<PCINT20);            // Mask out all except IRQ1
  IRQ1ActVal = IRQ1ActVal >> PCINT20;          // shift to right for bit0 position
  IRQ1ActTime=millis();                        // Read actual millis time
  if(IRQ1ActTime - IRQ1PrevTime > bounceTime)  // No bouncing anymore:
  {  
    // No contact bouncing anymore:
    if(IRQ1PrevVal==0 && IRQ1ActVal==1)        // Transition 0-->1
    {
      // Place your command for rising signal here...
      counter1++;
      if(counter1>255) counter1 = 0;
      IRQ1PrevTime=IRQ1ActTime;  
      IRQ1PrevVal=IRQ1ActVal;   
      irqFlag1=1;
    }
    if(IRQ1PrevVal==1 && IRQ1ActVal==0)        // Transition 1-->0
    {
      // Place your command for falling signal here... 
      IRQ1PrevVal=IRQ1ActVal;
    }
  }
}
ISR(PCINT1_vect)
{
  // You have to write your own interrupt handler. Don't change the name!
  // This code will be called anytime when PCINT23 switches high to low, 
  // or low to high.
  byte PVal;                                   // Port value (8 Bits)
  byte IRQ2ActVal;                             // Actual IRQ1 value
  long unsigned IRQ2ActTime;

  PVal = PINC;                                 // Read port D (8 bit)
  IRQ2ActVal = PVal & (1<<PCINT8);            // Mask out all except IRQ1
  IRQ2ActVal = IRQ2ActVal >> PCINT8;          // shift to right for bit0 position
  IRQ2ActTime=millis();                        // Read actual millis time
  if(IRQ2ActTime - IRQ2PrevTime > bounceTime)  // No bouncing anymore:
  {  
    // No contact bouncing anymore:
    if(IRQ2PrevVal==0 && IRQ2ActVal==1)        // Transition 0-->1
    {
      // Place your command for rising signal here...
      counter2++;
      if(counter2>255) counter2 = 0;
      IRQ2PrevTime=IRQ2ActTime;  
      IRQ2PrevVal=IRQ2ActVal;   
      irqFlag2=1;
    }
    if(IRQ2PrevVal==1 && IRQ2ActVal==0)        // Transition 1-->0
    {
      // Place your command for falling signal here... 
      IRQ2PrevVal=IRQ2ActVal;
    }
  }
}

