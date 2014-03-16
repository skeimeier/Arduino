//**************************************************************//
//  Name    : shiftOutCode, Hello World                                
//  Author  : Carlyn Maw,Tom Igoe, David A. Mellis 
//  Date    : 25 Oct, 2006    
//  Modified: 23 Mar 2010                                 
//  Version : 2.0                                             
//  Notes   : Code for using a 74HC595 Shift Register           //
//          : to count from 0 to 255                           
//****************************************************************

//Pin connected to ST_CP of 74HC595
int latchPin = 5;
//Pin connected to SH_CP of 74HC595
int clockPin = 3;
////Pin connected to DS of 74HC595
int dataPin = 2;
int enablePin = 4;
int clearPin = 6;

int halPin = 7;
int analogOutPin = 9; // Analog output pin that the LED is attached to
int outputValue = 10;

void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(halPin, INPUT);
  digitalWrite(halPin,HIGH);

}

void loop() {
  // count from 0 to 255 and display the number 
  // on the LEDs
  int hal = digitalRead(halPin);
  
    analogWrite(analogOutPin, outputValue);           
    outputValue += 10;
    outputValue  = (outputValue > 255) ? 10 : outputValue;
  if( hal == 0){
    for (int numberToDisplay = 1; numberToDisplay < 256; numberToDisplay=numberToDisplay*2) {
      // take the latchPin low so 
      // the LEDs don't change while you're sending in bits:
      digitalWrite(enablePin, HIGH);
      digitalWrite(latchPin, LOW);
      digitalWrite(clearPin, LOW);
      digitalWrite(clearPin,HIGH);
    
      // shift out the bits:
      shiftOut(dataPin, clockPin, MSBFIRST, numberToDisplay);  
    

      //take the latch pin high so the LEDs will light up:
      digitalWrite(latchPin, HIGH);
      //delay(1);
       digitalWrite(latchPin, LOW);
       digitalWrite(enablePin, LOW);
   
      // pause before next value:
      delay(100);
    }
  }else{
    for (int numberToDisplay = 1; numberToDisplay < 256; numberToDisplay=numberToDisplay*2) {
      // take the latchPin low so 
      // the LEDs don't change while you're sending in bits:
      digitalWrite(enablePin, HIGH);
      digitalWrite(latchPin, LOW);
      digitalWrite(clearPin, LOW);
      digitalWrite(clearPin,HIGH);
    
      // shift out the bits:
      shiftOut(dataPin, clockPin, LSBFIRST, numberToDisplay );  
    

      //take the latch pin high so the LEDs will light up:
      digitalWrite(latchPin, HIGH);
      //delay(1);
       digitalWrite(latchPin, LOW);
       digitalWrite(enablePin, LOW);
   
      // pause before next value:
      delay(100);
    }    
    
    
  }
}

