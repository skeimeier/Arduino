

#include <Ports.h>

PortI2C myI2C (1);
DeviceI2C clock (myI2C,0x50);  // clockchip = PCF8583


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
  //  setTime();
 
 
  }
  

}

void setTime(){
    Serial.println("SetTime");
     clock.send();
     clock.write(2);
     clock.write(0);  // sekunden
     clock.write(0x36); //minute
     clock.write(0x15);// hour
     clock.write(0x12);  // year_date
     clock.write(0x04  | (0x04<<5 )); //wday_month
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
    byte year_date = clock.read(1);
     clock.receive();

     byte wday_month = clock.read(1);
     
     
     clock.stop();
      Serial.print("Time: ");
      Serial.print(hour,HEX);
      Serial.print(":");
      Serial.print(minute,HEX);
      Serial.print(":");
      Serial.println(second,HEX);
      Serial.print("Date: ");
      Serial.print(wday_month>>5,HEX);
      Serial.print(" ");
      Serial.print(year_date & 0x3F,HEX);
       Serial.print(".");
      Serial.print((wday_month & 0x1F),HEX);
       Serial.print(".");
       Serial.print("20");
      Serial.println(0x12 + (year_date>>6)
      ,HEX);
  
}



