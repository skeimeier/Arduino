
/*
  Dimmer
 
 Demonstrates the sending data from the computer to the Arduino board,
 in this case to control the brightness of an LED.  The data is sent
 in individual bytes, each of which ranges from 0 to 255.  Arduino
 reads these bytes and uses them to set the brightness of the LED.
 
 The circuit:
 LED attached from digital pin 9 to ground.
 Serial connection to Processing, Max/MSP, or another serial application
 
 created 2006
 by David A. Mellis
 modified 30 Aug 2011
 by Tom Igoe and Scott Fitzgerald
 
 This example code is in the public domain.
 
 http://www.arduino.cc/en/Tutorial/Dimmer
 
 */
 
 
#include <EEPROM.h>

const int ledFET = 9;      
const int ledGN = 13;     
const int bwmSig = 8;  
const int eepromadr = 0;

int eepromdata;

boolean state, statelast,statechange,ledon;
byte brightness;
long ms;
long mslast;
long msdauer = 10000;
int seconds, minutes;

void dimup()
{    
  for(byte i = 0;i<255;i++){
        brightness = i;
        //Serial.println(brightness, HEX);
        analogWrite(ledFET, brightness);
        delay(10);
  }
  
  ledon = true;
}

void dimdn()
{
    for(int i = 255;i>=0;i--){
        brightness = i;
       // Serial.println(brightness, HEX);
        analogWrite(ledFET, brightness);
        delay(10);
    }
  ledon = false;

}

void setup()
{
  // initialize the serial communication:
  Serial.begin(9600);
  // initialize the ledPin as an output:
  pinMode(ledFET, OUTPUT);
  pinMode(ledGN, OUTPUT);
  pinMode(bwmSig, INPUT);
  digitalWrite(bwmSig,HIGH);
  brightness = 0;
 
  state = false;
  statelast = false;
  ledon = false;

  seconds = EEPROM.read(eepromadr);
  minutes = EEPROM.read(eepromadr+1);
  
  if( msdauer != (seconds + minutes*60)*1000 ){
      minutes = msdauer/1000/60;
      seconds = (msdauer/1000) - (minutes*60*1000);
 
   }
}

void loop() {
  

  if(digitalRead(bwmSig)==LOW){
            state = true;
        //Serial.println(state);
             digitalWrite(ledGN,HIGH);
  }else{
             digitalWrite(ledGN,LOW);
            state = false;
  }
  statechange = state != statelast;
  if(statechange){ 
     statelast = state; 
     mslast = millis();
     if( !ledon)
          dimup();     
  }
  if(ledon){
     ms = millis();    
     if( (ms - mslast) > msdauer ){
       dimdn();
      }
  }
  
}


