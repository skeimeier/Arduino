//**************************************************************//
//  Name    : shiftOutCode, Hello World                                
//  Author  : Carlyn Maw,Tom Igoe, David A. Mellis 
//  Date    : 25 Oct, 2006    
//  Modified: 23 Mar 2010                                 
//  Version : 2.0                                             
//  Notes   : Code for using a 74HC595 Shift Register           //
//          : to count from 0 to 255                           
//****************************************************************


#include <MsTimer2.h>


//Pin connected to ST_CP of 74HC595
int latchPin = 3;
//Pin connected to SH_CP of 74HC595
int clockPin = 14;
////Pin connected to DS of 74HC595
int dataPin = 4;

int analogPin = 3;     // potentiometer wiper (middle terminal) connected to analog pin 3



int outputValue[] = {0x00,0x00,0x00,0x00};
byte bitpos[] = {2,1,8,4,0x20,0x10,0x80,0x40};
byte vierer[] = {1,0,3,2};
static long count = 0;
volatile int showval = 1;
byte newVal = 0;
volatile uint8_t current_top_volume = 0;
volatile uint32_t speed = 0;
volatile uint32_t height = 0;

void setval(int value) {
  
  if ( value >30 ) value = 30;
  value -= 1;
  outputValue[0] = 0;
  outputValue[1] = 0;
  outputValue[2] = 0;
  outputValue[3] = 0;
  
  int block = value / 4;
  int bitnum = vierer[value % 4];
  outputValue[bitnum] = bitpos[block];
  
  

}

void setLed(int value) {
  
  if ( value >30 ) value = 30;
  value -= 1;
 
  int block = value / 4;
  int bitnum = vierer[value % 4];
  outputValue[bitnum] |= bitpos[block];
  
  

}

void clrLed(int value) {
  
  if ( value >30 ) value = 30;
  value -= 1;
 
  int block = value / 4;
  int bitnum = vierer[value % 4];
  outputValue[bitnum] &= ~bitpos[block];
  
  

}

void drop() {
    if (showval < current_top_volume) {
        // volume decreased recently
        // ensure top_volume LED is lit
           setLed(current_top_volume);
 
        // now let the top_volume indicator follow down
        ++speed;
        height += speed;
         
        if (height > 20000) {
            height-= 20000;
            clrLed(current_top_volume);
            --current_top_volume;
            setLed(current_top_volume);
        }
    }
}

void setbar(int value) {
  int block;
  int bitnum;     
  if ( value >30 ) value = 30;
      cli();

  outputValue[0] = 0;
  outputValue[1] = 0;
  outputValue[2] = 0;
  outputValue[3] = 0;
  value -= 1;
    if (showval >= current_top_volume) {
        current_top_volume = showval;
        speed = 0;
        height = 0;
    }  
  for(int i = 0;i<=value;i++){
     block = i / 4;
     bitnum = vierer[i % 4];
    outputValue[bitnum] |= bitpos[block];
  }
  setLed(current_top_volume);

  sei();

}

void setup() {
  //set pins to output so you can control the shift register
  Serial.begin(57600);

  Serial.println("\n[LED_BAR_595x2]");

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  digitalWrite(latchPin, LOW);
  
    MsTimer2::set(1, drop);
    MsTimer2::start();


}

void loop() {
   
    // send data only when you receive data:
    if (Serial.available() > 0) {
                // read the incoming byte:
                newVal = Serial.read();
                
    }
    showval = map(analogRead(analogPin),0,1023,0,30);

    for (int i = 0; i<4; i++) {
      // take the latchPin low so 
      // the LEDs don't change while you're sending in bits:
//      setval(showval);
      setbar(showval);
      // shift out the bits:
      shiftOut(dataPin, clockPin, MSBFIRST, 1 << i);  
      shiftOut(dataPin, clockPin, MSBFIRST, outputValue[i]);  
    

      //take the latch pin high so the LEDs will light up:
      digitalWrite(latchPin, HIGH);    
       digitalWrite(latchPin, LOW);
   
      // pause before next value:
    }
  /*  
    count +=1 ;
   
    if(count >= 5){
        count = 0;
        showval++; 
        if(showval > 30) showval= 0 ;
    }   
    
   */

}

