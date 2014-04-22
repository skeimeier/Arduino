// This example sends commands to the FS10 units via OOK at 433 Mhz.
// 2012-12-28 Steffi

// send FS10 command: <addr>,<dev>,<on>f
// addr : [0..7] ,  dev :[1..4] on : 0 = off , 1 = on
// 

#include <JeeLib.h>
#include <util/parity.h>

#define LED_PIN     5   // activity LED, comment out to disable

Port sender(1);
static byte value, stack[10], top;
 
 
static void activityLed (byte on) {
#ifdef LED_PIN
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, !on);
#endif
}


// Turn transmitter on or off, 
static void ookPulse(int on, int off) {
    sender.digiWrite(1);
    delayMicroseconds(on );
    sender.digiWrite(0);
    delayMicroseconds(off );
}

// prüfbit für den drei_bit daten bereich
// die Anzahl der "1" bits wird über die drei Datenbits gezählt 
// 
static byte parity(int val) {
       int count;
       for (byte bit = 0; bit < 3; ++bit) {
             count += bitRead(val, bit);
        }
    /*DEBUG
    Serial.print("count: "); 
    Serial.println(count); 
    */   
    return (!(count & 1)); 
}


// ein DatenBlock besteht aus 5 bits (b4 b3 b2 b1 b0)
// bit b4 ist prüfbit für den drei_bit daten bereich
// bits b3 b2 b1 sind die datenbits
// bit b0 ist immer 1
static byte block(int val) {
       int count;
       for (byte bit = 0; bit < 3; ++bit) {
             count += bitRead(val, bit);
        }
    return ( (parity(val)<<4) | (val << 1) | 1 ); 
}

static void FS10_Send(char addr, byte dev, byte on) {
  
  byte Blocks[6];
  // Aufbereitung 
   Blocks[0] = block(on);
   Blocks[1] = block(dev);
   Blocks[2] = block(1);
   Blocks[3] = block(0);
   Blocks[4] = block(addr);
   byte pruef = 18 - on - dev - 1 - addr;
   if(pruef >= 8) pruef = pruef - 8;
   Blocks[5] = block(pruef);

   /*DEBUG
   for(byte i = 0;i<6;i++){  
      Serial.println(Blocks[i],BIN); 
   }
   //*/
  
   //präambel
   for (byte i = 0; i < 12; ++i) {
       ookPulse(400,400);
   }
   ookPulse(400,800);
   // blocks 
    for (byte i = 0; i < 6; ++i) {
        for (byte bit = 5; bit > 0; bit--){
            //ookPulse(400,400);
            int on = bitRead(Blocks[i], bit-1) ? 800 : 400;
            ookPulse(400,on) ;
        }
    }

}

static void handleInput (char c) {
    if ('0' <= c && c <= '9')
        value = 10 * value + c - '0';
    else if (c == ',') {
        if (top < sizeof stack)
            stack[top++] = value;
        value = 0;
    } else if ('a' <= c && c <='z') {


            if( c == 'f') {  // send FS10 command: <addr>,<dev>,<on>f
                
                activityLed(1);
                FS10_Send(stack[0], stack[1], value);
                activityLed(0);
                //rf12_config(); // restore normal packet listening mode
            }
            value = top = 0;
            memset(stack, 0, sizeof stack);
            

    }
}

void setup() {
    Serial.begin(57600);
    Serial.println("\n[FS10_test]");
    sender.mode(OUTPUT);
    sender.digiWrite(0);

    //rf12_initialize(0, RF12_433MHZ);
}

void loop() {  
      if (Serial.available())
        handleInput(Serial.read());

  
    /*TEST
    Serial.println("off");
    FS10_Send(4, 1, 0);
    delay(2000);
    
    Serial.println("on");
    FS10_Send(4, 1, 1);
    delay(5000);
    */
}
