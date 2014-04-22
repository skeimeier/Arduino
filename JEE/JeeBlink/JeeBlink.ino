
/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
#include <Ports.h>

Port led (1);


void setup() {                
  Serial.begin(57600);

  Serial.println("\n[JeeBlink]");
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  led.mode( OUTPUT);     
}

void loop() {
  led.digiWrite(HIGH);   // set the LED on
  delay(100);              // wait for a second
  led.digiWrite(LOW);    // set the LED off
  delay(100);              // wait for a second
}

