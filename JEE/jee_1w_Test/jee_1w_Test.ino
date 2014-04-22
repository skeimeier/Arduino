
/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
#include <JeeLib.h>
//#include <Ports.h>
#include <OneWire.h>

//Port led (1);

OneWire ds18b20 (4); // 1-wire temperature sensors, uses DIO port 1
  byte oneID[8];

static uint8_t initOneWire () {
    if (ds18b20.search(oneID)) {
        Serial.print(" 1-wire");
        for (uint8_t i = 0; i < 8; ++i) {
            Serial.print(' ');
            Serial.print(oneID[i], HEX);
        }
        Serial.println();
    }
    ds18b20.reset();
}

static int readout1wire () {
    ds18b20.reset();
    ds18b20.skip();
    ds18b20.write(0x4E); // write to scratchpad
    ds18b20.write(0);
    ds18b20.write(0);
    ds18b20.write(0x1F); // 9-bits is enough, measurement takes 94 msec
    ds18b20.reset();
    ds18b20.skip();
    ds18b20.write(0x44, 0); // start conversion
    delay(100);
    ds18b20.reset();
    // ds18b20.select(oneID);
    ds18b20.skip();
    ds18b20.write(0xBE); // read scratchpad
    uint8_t data[9];
    for (uint8_t j = 0; j < 9; ++j)
        data[j] = ds18b20.read();
    ds18b20.reset();
    if (OneWire::crc8(data, 8) != data[8]) {
        Serial.println(" crc? ");
        return 0;
    }
    return ((data[1] << 8) + data[0]) * 10 >> 4; // degrees * 10
}

void setup() {
    Serial.begin(57600);
    Serial.print("\n[1W-Temp]");
 
    initOneWire();
}

void loop() {
       int temp = readout1wire();

         Serial.print(temp);
         Serial.println();
        delay(3000); // make sure tx buf is empty before going back to sleep

}


