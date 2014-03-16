/// @dir fs20demo
/// This example sends commands to the Conrad/ELV 868 MHz FS20 units via OOK.
// 2009-02-21 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

// Note thar RFM12B radios are not really designed for OOK (on-off keying),
// but this can be simulated anyway by simply turning the transmitter on and 
// off via the SPI interface. Doing so takes about 25 usecs, so the delays
// used for encoding simple bit patterns need to be adjusted accordingly.

#include <JeeLib.h>
#include <util/parity.h>
#include <OneWire.h>


//Port led (1);

OneWire ds18b20 (4); // 1-wire temperature sensors, uses DIO port 1
  byte oneID[8];
  int isttemp;
  int solltemp = 40;  // Grad Celsius * 10
  int hysterese = 20;  // Grad * 10
  
typedef struct {
    //byte restdauer;  // motion detector
    //byte leds_ramp;
    //byte light;     // light sensor
    //byte moved :1;  // motion detector
    //byte humi  :7;  // humidity
    int temp   ; // temperature
    // byte vcc1 ;  // supply voltage dropped under 2.2 V
    // byte vcc2 ;  // supply voltage dropped under 2.2 V
} Paket;

static Paket payload;


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





void sendBits(uint16_t data, uint8_t bits) {
    if (bits == 8) {
        ++bits;
        data = (data << 1) | parity_even_bit(data);
    }
    for (uint16_t mask = bit(bits-1); mask != 0; mask >>= 1) {
        // Timing values empirically obtained, and used to adjust for on/off
        // delay in the RF12. The actual on-the-air bit timing we're after is
        // 600/600us for 1 and 400/400us for 0 - but to achieve that the RFM12B
        // needs to be turned on a bit longer and off a bit less. In addition
        // there is about 25 uS overhead in sending the on/off command over SPI.
        // With thanks to JGJ Veken for his help in getting these values right.
        int width = data & mask ? 600 : 400;
        rf12_onOff(1);
        delayMicroseconds(width + 150);
        rf12_onOff(0);
        delayMicroseconds(width - 200);
    }
}

void fs20cmd(uint16_t house, uint8_t addr, uint8_t cmd) {
	uint8_t sum = 6 + (house >> 8) + house + addr + cmd;
	for (uint8_t i = 0; i < 3; ++i) {
		sendBits(1, 13);
		sendBits(house >> 8, 8);
		sendBits(house, 8);
		sendBits(addr, 8);
		sendBits(cmd, 8);
		sendBits(sum, 8);
		sendBits(0, 1);
		delay(10);
	}
}


static byte sendPayload () {
  
  payload.temp = isttemp;
  
  //rf12_sleep(RF12_WAKEUP);
  rf12_sendNow(0, &payload, sizeof payload);
  //rf12_sendWait(SEND_MODE);
  //rf12_sleep(RF12_SLEEP);

}

void setup() {
    Serial.begin(57600);
    Serial.println("\n[fs20_1W_Thermostat]");
    initOneWire();
  // intialize wireless
    rf12_initialize(14, RF12_868MHZ, 18);
    
   // rf12_initialize(0, RF12_868MHZ);
}



void loop() {  

 
    isttemp = readout1wire();
    Serial.println(isttemp);
    if( isttemp > (solltemp + hysterese) ) { 
      Serial.println("on");
    
      rf12_initialize(0, RF12_868MHZ);
      fs20cmd(0xfab4, 0, 17);
      rf12_initialize(14, RF12_868MHZ, 18);

    }
    if( isttemp <  (solltemp - hysterese) ) {
        Serial.println("off");
      rf12_initialize(0, RF12_868MHZ);
      fs20cmd(0xfab4, 0, 0);
      rf12_initialize(14, RF12_868MHZ, 18);
    }
    
    delay(10000);
   sendPayload ();
}
