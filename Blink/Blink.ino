/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
#define LED 9
void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(LED, OUTPUT);     
}

void loop() {
  digitalWrite(LED, HIGH);   // set the LED on
  delay(2000);              // wait for a second
  digitalWrite(LED, LOW);    // set the LED off
  delay(100);              // wait for a second
}

