#include <Ports.h>
#include <RF12.h>

Port ldr (3);

void setup () {
    // initialize the serial port and the RF12 driver
    Serial.begin(57600);
    Serial.print("\n[pof66]");
    rf12_config();
    // set up easy transmissions every 5 seconds
    rf12_easyInit(5);
    // enable pull-up on LDR analog input
    ldr.mode2(INPUT);
    ldr.digiWrite2(1);
}

void loop () {
    // keep the easy tranmission mechanism going
    rf12_easyPoll();
    // convert analog 0..1023 readings to a 0..255 light level
    byte value = 255 - ldr.anaRead() / 4;
    // send measurement data, but only when it changes
    rf12_easySend(&value, sizeof value);
}
