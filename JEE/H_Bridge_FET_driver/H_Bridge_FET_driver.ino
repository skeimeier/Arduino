/*
  einfacher FET H-Bridge Motortreiber 
  

 */

#include <JeeLib.h>
#include <avr/sleep.h>

#define MAX_DAUER 15

uint8_t nodeid; // nodeId as saved in EEPROM
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int a_side = 4;  // PORT 1 DIO
int b_side = 14; // PORT 1 AIO


MilliTimer motorTimer, minutenTimer, sendtimer;  //Pulsdauer für eine Motorbewegung (Regenrelay)

int BewaesserungsDauer = 0; // minuten dauer für Bewässerung
boolean dosend = false;

typedef struct {
    byte restdauer;  // motion detector
    //byte leds_ramp;
    //byte light;     // light sensor
    //byte moved :1;  // motion detector
    //byte humi  :7;  // humidity
    //int temp   :10; // temperature
     byte lobat ;  // supply voltage dropped under 2.2 V
} Paket;

static Paket payload;


// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(a_side, OUTPUT);     
  digitalWrite(a_side, LOW);   // turn the LED on (HIGH is the voltage level)
  pinMode(b_side, OUTPUT);     
  digitalWrite(b_side, LOW);   // turn the LED on (HIGH is the voltage level)
  
     Serial.begin(57600);
    Serial.println(57600);
    Serial.println("H-Bridge_Wasser");
    //rf12_initialize(13, RF12_868MHZ, 18);
    nodeid = rf12_config( true ); // nodeId as saved in EEPROM
    rf12_control(0xC040); // set low-battery level to 2.2V i.s.o. 3.1V

    rf12_easyInit (0);
    

}

void mot_bwd() {
  digitalWrite(a_side, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(b_side, HIGH);   // turn the LED on (HIGH is the voltage level) 
  motorTimer.set(800); //one-shot timer 800 ms
   Serial.println("Wasser stopp");
}

void mot_fwd() {
  digitalWrite(a_side, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(b_side, LOW);   // turn the LED on (HIGH is the voltage level) 
   motorTimer.set(800);  //one-shot timer 800 ms
   Serial.println("Wasser marsch");

}

void mot_off() {
  digitalWrite(a_side, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(b_side, LOW);   // turn the LED on (HIGH is the voltage level) 
}

void setDauer(int len){
   BewaesserungsDauer = (len < MAX_DAUER) ? len : MAX_DAUER;  
   if(BewaesserungsDauer)  // wenn grösser null
         mot_fwd();   // motorimpuls für Öffen starten
   else
          mot_bwd();   // motorimpuls für schliessen
   minutenTimer.set(60000);
   dosend = true;
   
}


static void handleInput (char c) {
  if ('0' <= c && c <= '9')
    setDauer( c - '0');
}

// the loop routine runs over and over again forever:
void loop() {
  if(minutenTimer.poll(60000)){
    dosend = true;
    if(BewaesserungsDauer)
        BewaesserungsDauer--; 
    if( BewaesserungsDauer == 0 )
        mot_bwd();  //// motorimpuls für Schliessen starten 
  }
  
  if (Serial.available())
    handleInput(Serial.read());


  if (rf12_recvDone() && rf12_crc == 0 &&  (rf12_hdr & RF12_HDR_MASK) == nodeid) {
    const byte* p = (const byte*) rf12_data;

    if (rf12_len == 1) {
      // a single byte loads the ramp from EEPROM
      setDauer(rf12_data[0]);
    }
    if (RF12_WANTS_ACK)	
            rf12_sendStart(RF12_ACK_REPLY, 0, 0);

  }
  if ( sendtimer.poll(1000) && dosend && rf12_canSend()) {
     payload.restdauer = BewaesserungsDauer; 
     payload.lobat = rf12_lowbat();
     dosend = false;
     rf12_sendStart(RF12_HDR_ACK, &payload, sizeof payload);
  }
  
    
  if(motorTimer.poll())
    mot_off();
  

}
