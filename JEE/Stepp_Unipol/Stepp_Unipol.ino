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
int latchPin = 8;
//Pin connected to SH_CP of 74HC595
int clockPin = 12;
////Pin connected to DS of 74HC595
int dataPin = 11;
int dirPin = 7;
int pattern[4];

int dir = 0;

void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(dirPin, INPUT);
  pattern[0] = 0b0011;
  pattern[1] = 0b0110;
  pattern[2] = 0b1100;
  pattern[3] = 0b1001;
  
}

void loop() {
  // count from 0 to 255 and display the number 
  // on the LEDs
  for(int i = 0 ; i <25 ; i++){
    for (int numberToDisplay = 0; numberToDisplay < 4; numberToDisplay++) {
      // take the latchPin low so 
      // the LEDs don't change while you're sending in bits:
      digitalWrite(latchPin, LOW);
      // shift out the bits:
      dir = digitalRead(dirPin);
      if( dir){ 
          shiftOut(dataPin, clockPin, MSBFIRST, pattern[3-numberToDisplay]<<4);
      }else{
          shiftOut(dataPin, clockPin, MSBFIRST, pattern[numberToDisplay]<<4);
      }

      //take the latch pin high so the LEDs will light up:
      digitalWrite(latchPin, HIGH);
      // pause before next value:
      delay(25);
    }  
  }
    delay(1000);
}

