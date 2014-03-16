#include <Ports.h>

Port Adr0(4);
Port Adr1(3);

Port Wrt(1);

const int pinCount = 6;    // the number of LEDs in the bar graph

int dataPins[] = { 
  14, 15, 16, 17, 18, 19};   // an array of pin numbers to which LEDs are attached

void chrOut(char c) {
  Serial.println(c);
  for (int thisPin = 0; thisPin < pinCount; thisPin++) {
    // if the array element's index is less than ledLevel,
    // turn the pin for this element on:
    if (c & (_BV(thisPin) ) ) {
      digitalWrite(dataPins[thisPin], HIGH);
    }else{ 
      digitalWrite(dataPins[thisPin], LOW);
    };
  };
  Wrt.digiWrite(LOW);   //
  delay(1);              // wait  a milisecond
  Wrt.digiWrite(HIGH);    // 
}




void setup() {
  Serial.begin(57600);

  Serial.println("\n[Dl1414_test]");
  // loop over the pin array and set them all to output:
  for (int thisPin = 0; thisPin < pinCount; thisPin++) {
    pinMode(dataPins[thisPin], OUTPUT); 
  }
  Wrt.mode(OUTPUT);
  
}




void loop() {
  
  
  chrOut('0');
  delay(2000);
 chrOut('1');
  delay(2000);
 chrOut('2');
  delay(2000);
 chrOut('3');
  delay(2000);
 chrOut('4');
  delay(2000);
  chrOut('5');
  delay(2000);

  }

