//
// inslude the SPI library:
#include <SPI.h>

void setup() {

	//Port B.3 SDI auf Output
        SPI.begin(); 
}


void loop() {

	//Ausgabe LED 0-7
	SPI.transfer(0xaa);
	delay(1000);
	SPI.transfer(0xaa);
	delay(1000);
	SPI.transfer(0x55);
	delay(1000);
	SPI.transfer(0x55);
	delay(1000);
	
	//Endlosschleife
}

