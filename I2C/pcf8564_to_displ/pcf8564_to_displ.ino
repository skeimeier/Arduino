

#include <JeeLib.h>
#include <Ports.h>
#include <RF12.h>


//>>> The latest version of this code can be found at https://github.com/jcw/ !!

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
// 2010-02-04 <jcw@equi4.com> http://opensource.org/licenses/mit-license.php
// $Id: pcf8563.pde 7763 2011-12-11 01:28:16Z jcw $

// contributed by @mariusster, see http://forum.jeelabs.net/comment/1902


#include <RTClib2.h>
//#include <Ports.h>

PortI2C clockI2C (1);

String ausgabe = "0123456789012345";


RTC_PCF8564 RTC(clockI2C,0x50); 

PortI2C dispI2C (2);


DeviceI2C DL1414 (dispI2C,0x20);



enum { 
  MCP_GP0, MCP_GP1, MCP_OLAT0, MCP_OLAT1, MCP_IPOL0,
  MCP_IPOL1, MCP_IODIR0, MCP_IODIR1, MCP_INTCAP0,
  MCP_INTCAP1,MCP_IOCON0, MCP_IOCON1
};

char data = 0;
int controll = 0xff;
int numcols = 16;
int pos = 0;

void DL1414_write(char data){
  DL1414.send();
  DL1414.write(MCP_GP1);
  //delayMicroseconds(20);
  DL1414.write(data);
  delayMicroseconds(20);
  DL1414.write(controll);
  //delayMicroseconds(20);
  DL1414.stop();

}

void DL1414_set_adr(){
  byte adress =  3 -  (pos & 0x03);
  //Serial.print("pos: ");
 // Serial.print(pos);
  // Serial.print("  adr: ");
  //Serial.print(adress,HEX);

 controll = 0b11111100 | adress;
   //Serial.print(" controll_1: ");
 
  //Serial.print(controll,BIN);
 
   DL1414_write(data); 
}


void DL1414_set_displ(){
  char displNo = pos >> 2;
  char thisbit = _BV(displNo);
 // Serial.print("wrbits: ");
  //Serial.println(thisbit,BIN);
  controll &=  ~(thisbit<<2)  ;
 DL1414_write(data); 
    //Serial.print(" controll_2: ");
 
 // Serial.print(controll,BIN);
 
  // DL1414_write(data); 
} 
  



void DL1414_charOut(char c){
  DL1414_set_adr();
  DL1414_set_displ();
   data = c;
  //Serial.print(" Chr: " );
  // Serial.print( c );
   DL1414_set_adr();
   // Serial.println();
 
  if(pos < (numcols-1) ) pos++;
} 

void DL1414_clear(){
    //Serial.println("Clear    ");
   pos = 0;
 for(int i = 0; i< numcols;i++ ){
      DL1414_charOut(' ');
  };
  pos = 0;
}

String numExpand(byte num){
   String twochr = "xx";
   twochr.setCharAt(1,'0'+(num%10));
   twochr.setCharAt(0,'0'+(num/10));
   
   return twochr;
  
}



String monthToStr(byte m){
   String month = "";
   switch (m){
     case 1: month = "JAN";
          break;
     case 2: month = "FEB";
          break;
     case 3: month = "MAR";
          break;
     case 4: month = "APR";
          break;
     case 5: month = "MAI";
          break;
     case 6: month = "JUN";
          break;
     case 7: month = "JUL";
          break;
     case 8: month = "AUG";
          break;
     case 9: month = "SEP";
          break;
     case 10: month = "OKT";
          break;
     case 11: month = "NOV";
          break;
     case 12: month = "DEZ";
          break;
   }   
   return month;
  
}

String wdayToStr(byte d){
   String wday = "";
   switch (d){
     case 0: wday = "SO";
          break;
     case 1: wday = "MO";
          break;
     case 2: wday = "DI";
          break;
     case 3: wday = "MI";
          break;
     case 4: wday = "DO";
          break;
     case 5: wday = "FR";
          break;
     case 6: wday = "SA";
          break;
   }   
   return wday;
  
}

void setup () {
    Serial.begin(57600);
   
      bool ok = DL1414.isPresent();
  Serial.println(ok?"da":"weg");
  if(ok){
    Serial.println("Init");
    DL1414.send();
    DL1414.write(MCP_IODIR0);   //IO -Register auf Ausgang
    delayMicroseconds(20);
    DL1414.write(0x00);
    delayMicroseconds(20);
    DL1414.write(0x00);
    delayMicroseconds(20);
    DL1414.stop();
    
    DL1414_clear();
    delay(1000);
   }
    pos = 0;
 
    
    
    // following line sets the RTC to the date & time this sketch was compiled
    //   RTC.adjust(DateTime(__DATE__, __TIME__));
    
}

void loop () {
    DateTime now = RTC.now();
    
    String Datum;
    String Zeit;
    
    uint16_t yy = 2012;
    
    Datum = "";
    
    Datum += wdayToStr(now.dayOfWeek());
    Datum += " ";
    Datum += numExpand(now.day());
    Datum += ".";    
    Datum +=  monthToStr(now.month()) ;
    Datum += " ";
    Datum += now.year();
    Datum += " ";

   pos = 1;
   for(int i = 0; i< numcols;i++ ){
      DL1414_charOut(Datum.charAt(i));
   };
   pos = 0;
    delay(1000);

    Serial.print(Datum);
     Zeit = "    ";
    Zeit += numExpand(now.hour());
    Zeit += ":";
    Zeit += numExpand(now.minute());
    Zeit += ":";
    Zeit += numExpand(now.second());
      Zeit += "    ";
    for(int i = 0; i< Zeit.length() ;i++ ){
      DL1414_charOut(Zeit.charAt(i));
   };
   pos = 0;
  
    
    
    
    
    Serial.println(Zeit);
    
     
    delay(1000);
}
