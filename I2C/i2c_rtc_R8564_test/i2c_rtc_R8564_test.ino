

#include <Ports.h>

PortI2C myI2C (1);
DeviceI2C clock (myI2C,0x51);  // clockchip = r8564je


void setup() {
  Serial.begin(57600);

  Serial.println("\n[clock]");
  bool ok = clock.isPresent();
  Serial.println(ok?"da":"weg");
  if(ok){
    Serial.println("Init");
     clock.send();
     clock.write(2);
     clock.receive();
     byte result = clock.read(1);
     clock.stop();
      Serial.println(result);
  //
  
  
    //setTime();
 
 
  }
  

}

void setTime(){
    Serial.println("SetTime");
     clock.send();
     clock.write(0); // Register 0
     clock.write(0b00100000);// Stop
     clock.write(0);
    
     clock.write(0);  // sekunden
     clock.write(0x45); //minute
     clock.write(0x09);// hour
     clock.write(0x06);  // days
     clock.write(0x06);  // wday
     clock.write(0x7);  // month
     clock.write(0x12);  // year
      clock.stop();
      
     clock.send();
     clock.write(0); // Register 0
     clock.write(0);// Stop sua
     clock.stop();
       
      Serial.println("Done");


}


void loop() {	
           delay(1000);

     clock.send();
     clock.write(2);
     clock.receive();
     byte second = clock.read(1);
     clock.receive();
     byte minute = clock.read(1);
     clock.receive();
     byte hour = clock.read(1);
      clock.receive();
    byte day = clock.read(1);
     clock.receive();
    byte wday = clock.read(1);
     clock.receive();
    byte month = clock.read(1);
     clock.receive();
    byte year = clock.read(1);
     clock.receive();
     
     
     clock.stop();
      Serial.print("Time: ");
      Serial.print(hour & 0b00111111,HEX);
      Serial.print(":");
      Serial.print(minute & 0b01111111,HEX);
      Serial.print(":");
      Serial.println(second & 0b01111111,HEX);
      Serial.print("Date: ");
      Serial.print(wday & 0b111,HEX);
      Serial.print(" ");
      Serial.print(day & 0b00111111,HEX);
       Serial.print(".");
      Serial.print((month & 0b00011111),HEX);
       Serial.print(".");
       Serial.print("20");
      Serial.println(year ,HEX);
  
}



