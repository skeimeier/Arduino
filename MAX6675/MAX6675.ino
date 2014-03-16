/*
 SCP1000 Barometric Pressure Sensor Display
 
 Shows the output of a Barometric Pressure Sensor on a
 Uses the SPI library. For details on the sensor, see:
 http://www.sparkfun.com/commerce/product_info.php?products_id=8161
 http://www.vti.fi/en/support/obsolete_products/pressure_sensors/
 
 This sketch adapted from Nathan Seidle's SCP1000 example for PIC:
 http://www.sparkfun.com/datasheets/Sensors/SCP1000-Testing.zip
 
 Circuit:
 MAX6675 sensor attached to pins  7, 12, 13:
 CSB: pin 7
 MISO: pin 12
 SCK: pin 13
 
 */

// the sensor communicates using SPI, so include the library:
#include <SPI.h>

const int chipSelectPin = 7;

void setup() {
  Serial.begin(57600);

  Serial.println("\n[ThermoCoupple SPI MAX6675]");

  // start the SPI library:
  SPI.begin();

  // initalize chip select pins:
  pinMode(chipSelectPin, OUTPUT);
}

void loop() {
  byte inByteHi = 0;           // incoming byte from the SPI
  byte inByteLo = 0;           // incoming byte from the SPI

  // take the chip select low to select the device:
  digitalWrite(chipSelectPin, LOW);
  // send a value of 0 to read the first byte returned:
  inByteHi = SPI.transfer(0x00);
  inByteLo = SPI.transfer(0x00);
  // take the chip select high to de-select:
  digitalWrite(chipSelectPin, HIGH);
  // return the result:
 
  Serial.println(); 
  Serial.print((((inByteHi*0x100)+inByteLo)>>3)/4.0);
       delay(300);
  


}
