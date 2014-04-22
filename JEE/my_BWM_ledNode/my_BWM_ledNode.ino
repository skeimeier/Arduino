// Programmable color ramps for the LED drivers on the LED Node
// 2011-10-26 <jcw@equi4.com> http://opensource.org/licenses/mit-license.php

#include <JeeLib.h>
#include <EEPROM.h>
#include <avr/sleep.h>

#define EEPROM_BASE 0x100 // store ramps starting at this offset
#define RAMP_LIMIT 100    // room for ramps 0..99, stored in EEPROM

#define LED_R 5   // the PWM pin which drives the red LED
#define LED_G 3   // the PWM pin which drives the green LED
#define LED_B 6   // the PWM pin which drives the blue LED

#define BWMSIG 4  // Pin für Bewegungsmelder
#define EEPROM_EINZEIT 0xF8 // speicher für Ein_Zeit ohne weitere Aktivität

boolean state, statelast,statechange,ledon, dosend;
byte seconds, minutes, wait_sec, wait_min;

uint8_t nodeid; // nodeId as saved in EEPROM


typedef struct {
    byte motion;  // motion detector
    byte leds_ramp;
    //byte light;     // light sensor
    //byte moved :1;  // motion detector
    //byte humi  :7;  // humidity
    //int temp   :10; // temperature
    //byte lobat :1;  // supply voltage dropped under 3.1V
} Paket;

static Paket payload;

// a "Ramp" is a target RGB color and the time in seconds to reach that color
// ramps can be chained together with a non-zero ramp index in the chain field
typedef struct {
  byte colors[3]; // red, green, blue, 0..255
  byte steps;     // number of seconds used to reach given RGB colors
  byte chain;     // next ramp to use when done, or 0 to stay as is
} Ramp;

long now[3];      // current PWM values, as 9+23 bit fractional int
long delta[3];    // current PWM deltas, as 9+23 bit fractional int
word duration;    // number of 0.01s steps remaining in this ramp
byte nextRamp;    // which saved ramp to use next (or none if zero)
MilliTimer timer; // used as timer for the 0.01s steps
MilliTimer ein_zeit; // used as timer for the 0.01s steps
MilliTimer sendtimer; // used as timer for the 0.01s steps


static Ramp stdRamps[] = {
  {   0,   0,   0, 0, 0 }, // 0: instant off
  { 255,   255,   255, 2, 0 }, // 1: on 
  {   0,   0,   0, 5, 0 }, // 2: off
  {   0,   0,   0, 5, 0 }, // 3: 5s off
  { 255,  85,  30, 5, 0 }, // 4: 5s warm white
  { 255, 150,  75, 5, 0 }, // 5: 5s cold white
  { 255,   0,   0, 5, 7 }, // 6: 5s red -> green -> blue
  {   0, 255,   0, 5, 8 }, // 7: 5s green -> blue -> red
  {   0,   0, 255, 5, 6 }, // 8: 5s blue -> red -> green
  {   7,   1,   0, 0, 0 } // 9: instant faint red'ish yellow
};

static void setLeds () {
  // set to bits 30..23, but rounded by one extra bit (i.e. bit 22)
  analogWrite(LED_R, (byte) (((word) (now[0] >> 22) + 1) >> 1));
  analogWrite(LED_G, (byte) (((word) (now[1] >> 22) + 1) >> 1));
  analogWrite(LED_B, (byte) (((word) (now[2] >> 22) + 1) >> 1));
}

static void useRamp (const void* ptr) {
  const Ramp* ramp = (const Ramp*) ptr;
  nextRamp = ramp->chain;
  duration = ramp->steps * 100;
  for (byte i = 0; i < 3; ++i) {
    long target = (long) ramp->colors[i] << 23;
    if (duration > 0)
      delta[i] = (target - now[i]) / duration;
    else
      now[i] = target;
  }
  setLeds();
}

static void loadRamp (byte pos) {
  if (pos < RAMP_LIMIT) {
    word addr = EEPROM_BASE + pos * sizeof (Ramp);
    Ramp ramp;
    for (byte i = 0; i < sizeof (Ramp); ++i)
      ((byte*) &ramp)[i] = EEPROM.read(addr+i);
    useRamp(&ramp);
  }
}

static void saveRamp (byte pos, const void* data) {
  if (pos < RAMP_LIMIT) {
    word addr = EEPROM_BASE + pos * sizeof (Ramp);
    for (byte i = 0; i < sizeof (Ramp); ++i)
      EEPROM.write(addr+i, ((const byte*) data)[i]);
  }
}

void dimup()
{    
   loadRamp(1); 
  ledon = true;
  payload.leds_ramp = 1;
  dosend = true;
}

void dimdn()
{
   loadRamp(2);
  ledon = false;
  payload.leds_ramp = 2;
  dosend = true;
}


void setup () {
    Serial.begin(57600);
    Serial.println("\n[BWM-led-node]");
  // fix timer 1 so it also runs in fast PWM mode, to match timer 0
  bitSet(TCCR1B, WGM12);
  // set up the default ramps
  for (byte i = 0; i < sizeof stdRamps / sizeof *stdRamps; ++i)
    saveRamp(i, stdRamps + i);
  // intialize wireless
  //rf12_initialize(1, RF12_868MHZ, 19);
  nodeid = rf12_config();
  // test code: start up with ramp #1
  loadRamp(0);
  
  // Pinns konfigurieren
  pinMode(BWMSIG, INPUT);
  digitalWrite(BWMSIG,HIGH); //Pullup
  state = false;
  statelast = false;
  ledon = false;
  dosend = false;
  
  //seconds = 10;
  //minutes = 0;
  //EEPROM.write(EEPROM_EINZEIT, seconds);
  //EEPROM.write(EEPROM_EINZEIT+1, minutes);

  seconds = EEPROM.read(EEPROM_EINZEIT);
  minutes = EEPROM.read(EEPROM_EINZEIT+1);

}

void loop () {
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_mode();

  // BWM lesen 
  if(digitalRead(BWMSIG)==LOW){
            state = true;
        //Serial.println(state);
          //   digitalWrite(ledGN,HIGH);
  }else{
            // digitalWrite(ledGN,LOW);
            state = false;
  }
  statechange = state != statelast;
  if(statechange){ 
     statelast = state; 
     wait_sec = seconds;
     wait_min = minutes;
     payload.motion = state;
     dosend = true;
     if( !ledon)
          dimup();     
  }
  if (ein_zeit.poll(1000)) {
      if(ledon){
        if(wait_sec > 0){
            --wait_sec;
        }else{
          if(wait_min > 0){
            wait_sec = 60;
            --wait_min;
          }else{
                 dimdn();
          }
        }
      }
  }
  if (timer.poll(10)) {
    if (duration > 0) {
      --duration;
      for (byte i = 0; i < 3; ++i)
        now[i] += delta[i];
      setLeds();
    } else if (nextRamp != 0)
      loadRamp(nextRamp);
  }
  
  if (rf12_recvDone() && rf12_crc == 0  
            &&  (rf12_hdr & RF12_HDR_MASK) == nodeid) {
    const byte* p = (const byte*) rf12_data;

    if (rf12_len == 1) {
      // a single byte loads the ramp from EEPROM
      loadRamp(rf12_data[0]);
    }else if (rf12_len == 2) {
      // zwei byte befehl ändert die Minuten , Sekunden für Eindauer
       seconds = rf12_data[0];
       minutes = rf12_data[1];
       EEPROM.write(EEPROM_EINZEIT, seconds);
       EEPROM.write(EEPROM_EINZEIT+1, minutes);
    } else if (rf12_len == 1 + sizeof (Ramp)) {
      // 6 bytes, either a save or an immediate command
      // make sure that slot zero, i.e. "all-off", is never overwritten
      if (rf12_data[0] > 0) {// save the date to EEPROM, if slot is not zero
        saveRamp(rf12_data[0], (const Ramp*) (rf12_data + 1));
      }
      else{ // use the ramp as is without saving, if slot is zero
         for (byte i = 0; i < sizeof (Ramp); ++i){
        }
        useRamp((const Ramp*) (rf12_data + 1));
      }
    }
    if (RF12_WANTS_ACK)	
            rf12_sendStart(RF12_ACK_REPLY, 0, 0);

  }
  if ( sendtimer.poll(1000) && dosend && rf12_canSend()) {
     dosend = false;
     rf12_sendStart(RF12_HDR_ACK, &payload, sizeof payload);
  }
  
}
