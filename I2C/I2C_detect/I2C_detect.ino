#include <Ports.h>


PortI2C myI2C (1);


void setup() {
  Serial.begin(57600);

  Serial.println("\n[I2C_detect]");

 
  }
  

void printhex(byte x){
  Serial.print( x>>4,HEX); 
  Serial.print( x & 0x0f,HEX);
}



void loop() {	
  static byte count = 0;

               Serial.println(); 
               Serial.println(); 

     delay(1000);
     for(count = 0;count <= 0x7F;count++){
      delay(1);
        if(count == 0) Serial.println("   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
         if(!( count & 0x0F) ){
               if(count != 0 ) Serial.println(); 
               printhex(count);
               Serial.print(" ");
         }      
         if( myI2C.start(count<<1) ){
               myI2C.stop(); 
               printhex(count);
               Serial.print(" ");
         
           }else{
              myI2C.stop(); 
             Serial.print("-- ");
           }
    }
}




