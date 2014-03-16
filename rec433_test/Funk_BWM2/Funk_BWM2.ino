// Test 433 MHz receiver
// Project home page is http://lab.equi4.com/files/wireless433.php
//
// 2008-11-27  v1  public release by Jean-Claude Wippler as MIT-licensed OSS
// 2008-12-02      don't display past received bytes, ignore empty/noise packets
// 2008-12-03      omit internal pullup, much better sync start bytes and logic
//
// Hardware setup: Arduino with Conrad 433 MHz receiver connected as follows:
//
//    +----------------------------+
//    |                         () +-- Antenna
//    |                            |
//    +-+---+--------------------+-+
//      |   |                    |
//     GND +5V               DATA/D10
//
// Sample output with two good receptions and a bad one:
// Arduino PIN10 (MEGA328 PORTB2) als Eingang
// now on JeeNode DIO1  = PD4
#include <util/crc16.h>

#define RXDATA 14
#define RX_PIN (PIND & _BV(PIND4))

unsigned int rxdata;
unsigned int bytes[22];
int crc, errors,cnt;
unsigned int last;

char state;


void setup() {
  Serial.begin(115200);
  //Serial.begin(9600);
  Serial.println("\n[RECV433_BWM.2]");
  pinMode(RXDATA, INPUT);
}

int sync() {
  unsigned int space = 0;
  unsigned int pulse = 0;
  while (RX_PIN)
  ;
  last = micros(); 
  
  while (!RX_PIN)
    ;
  space = micros() - last;
  //if(space < 100)   return pulse;
  
  last = micros(); 
  while (RX_PIN)
  ;
    pulse = micros() - last;
     // Serial.println(pulse,DEC);

  last = micros(); 
  return pulse;
}

char readBit() {
  bytes[cnt++] = 370 - (micros() - last);  
  delayMicroseconds(335);
  rxdata = (rxdata << 1) | (RX_PIN != 0);
  state = RX_PIN != 0;
  delayMicroseconds(200);
  if (!RX_PIN)
    ++errors;
  while (RX_PIN)
    ;
  last = micros(); 
  return rxdata;
}

void loop() {
  int leadin, j, n;
  rxdata = 0xFFFF;
  errors = 0;
    while ((leadin = sync()) < 185 || leadin > 205) // 235us am sender gemessen 
    ;
    cnt = 0;
    for ( j = 0; j < 12; ++j){
      readBit();
      if (errors)          break;
    }
  if(j == 0 ) return;
  rxdata &= 0x0FFF;
  if(j==12){

    /*Serial.print(' ');
    Serial.print(leadin);     // number of msecs in leadin
    Serial.print(':');
    Serial.print(j);          // number of bits received
    Serial.print(' ');
    Serial.print(rxdata,BIN);        // calculated crc value, will be 0 if data is ok
    Serial.print(' ');
    Serial.print(rxdata,HEX);     // number of errors in second bit half
    Serial.println();
  */
    if( (rxdata == 0xC08) ||  (rxdata == 0x808) ){
      Serial.print("OK ----> BWM-0000 ");
      Serial.print(rxdata,HEX);
      Serial.println();
    }
    if( (rxdata == 0x908) ||  (rxdata == 0xD08) ){
      Serial.print("OK ----> BWM-0001 ");
      Serial.print(rxdata,HEX);
      Serial.println();
    }
  }
}

