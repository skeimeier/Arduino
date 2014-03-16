/*
  einfacher FET H-Bridge Motortreiber 
  

 */

#include <JeeLib.h>
#include <avr/sleep.h>

#define MAX_DAUER 15
#define SEND_MODE 2   // set to 3 if fuses are e=06/h=DE/l=CE, else set to 2

uint8_t nodeid; // nodeId as saved in EEPROM
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int a_side = 4;  // PORT 1 DIO
int b_side = 14; // PORT 1 AIO
int led_a = 5;  //Testled
int led_b = 15;  //Testled

long  zehntelsekundenTimer, minutenTimer, sendtime, antwortTimer, blinkTimer;  //Pulsdauer für eine Motorbewegung (Regenrelay)

int BewaesserungsDauer = 0; // minuten dauer für Bewässerung
boolean dosend = false;
boolean done = false;

typedef struct {
    byte restdauer;  // motion detector
    //byte leds_ramp;
    //byte light;     // light sensor
    //byte moved :1;  // motion detector
    //byte humi  :7;  // humidity
    //int temp   :10; // temperature
     byte vcc1 ;  // supply voltage dropped under 2.2 V
     byte vcc2 ;  // supply voltage dropped under 2.2 V
} Paket;

static Paket payload;

volatile bool adcDone;

// for low-noise/-power ADC readouts, we'll use ADC completion interrupts
ISR(ADC_vect) { adcDone = true; }

// this must be defined since we're using the watchdog for low-power waiting
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

static byte vccRead (byte count =4) {
  set_sleep_mode(SLEEP_MODE_ADC);
  // use VCC as AREF and internal bandgap as input
#if defined(__AVR_ATtiny84__)
  ADMUX = 33;
#else
  ADMUX = bit(REFS0) | 14;
#endif
  bitSet(ADCSRA, ADIE);
  while (count-- > 0) {
    adcDone = false;
    while (!adcDone)
      sleep_mode();
  }
  bitClear(ADCSRA, ADIE);  
  // convert ADC readings to fit in one byte, i.e. 20 mV steps:
  //  1.0V = 0, 1.8V = 40, 3.3V = 115, 5.0V = 200, 6.0V = 250
  return (55U * 1024U) / (ADC + 1) - 50;
}





// the setup routine runs once when you press reset:
void setup() {   
  cli();
  CLKPR = bit(CLKPCE);
#if defined(__AVR_ATtiny84__)
  CLKPR = 0; // div 1, i.e. speed up to 8 MHz
#else
  CLKPR = 1; // div 2, i.e. slow down to 8 MHz
#endif
  sei();

  
  // initialize the digital pin as an output.
  pinMode(a_side, OUTPUT);     
  digitalWrite(a_side, LOW);   // turn the LED on (HIGH is the voltage level)
  pinMode(b_side, OUTPUT);     
  digitalWrite(b_side, LOW);   // turn the LED on (HIGH is the voltage level)
 pinMode(led_a, OUTPUT);
 pinMode(led_b, OUTPUT);
    // Serial.begin(57600);
    //Serial.println(57600);
    //Serial.println("H-Bridge_Wasser");
    //Serial.flush();
    //rf12_initialize(13, RF12_868MHZ, 18);
    nodeid = rf12_config( true ); // nodeId as saved in EEPROM
    rf12_control(0xC040); // set low-battery level to 2.2V i.s.o. 3.1V
  rf12_sleep(RF12_SLEEP);

    //rf12_easyInit (0);
     zehntelsekundenTimer = 600;  // eine Minute
   

}

void mot_bwd() {
  digitalWrite(a_side, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(b_side, HIGH);   // turn the LED on (HIGH is the voltage level) 
  Sleepy::loseSomeTime(800); // 800 ms warten
   //Serial.println("Wasser stopp");
   mot_off();
}

void mot_fwd() {
  digitalWrite(a_side, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(b_side, LOW);   // turn the LED on (HIGH is the voltage level) 
   Sleepy::loseSomeTime(800); // 800 ms warten
   //Serial.println("Wasser marsch");
   mot_off();

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
   dosend = true;
   
}


static void handleInput (char c) {
  if ('0' <= c && c <= '9')
    setDauer( c - '0');
}


static byte sendPayload () {
  
  payload.vcc1 = vccRead(); 
  payload.restdauer = BewaesserungsDauer;
  
  rf12_sleep(RF12_WAKEUP);
  rf12_sendNow(0, &payload, sizeof payload);
  rf12_sendWait(SEND_MODE);
  //rf12_sleep(RF12_SLEEP);

}

void doLedBlink()
{
   if( (zehntelsekundenTimer % 20) == 5 ){  // LED ein  Grün wenn Ventil zu / Rot wenn Bewaesserung läuft
              digitalWrite(led_a, (BewaesserungsDauer != 0) ? LOW : HIGH); 
              digitalWrite(led_b, (BewaesserungsDauer != 0) ? HIGH : LOW); 
   }
    if( (zehntelsekundenTimer % 20) == 4 ){ // LED aus
              digitalWrite(led_a,  HIGH ); 
              digitalWrite(led_b,  HIGH ); 
   }  
}

// the loop routine runs over and over again forever:
void loop() {
  
  if(zehntelsekundenTimer-- > 0){
    Sleepy::loseSomeTime(100);  // eine zehntelsekunde 
    doLedBlink();
  }else{ //jede minute
    if(BewaesserungsDauer != 0){
        BewaesserungsDauer--; 
        if( BewaesserungsDauer == 0 )
          mot_bwd();  //// motorimpuls für Schliessen starten 
    }   
    sendPayload();  // minütlicher Report
    antwortTimer = 100000; // dann 1000 ms warten ob Receiver mit neuem Befehl antwortet
    done = false;
    while(!done){
          if (rf12_recvDone() && rf12_crc == 0 &&  (rf12_hdr & RF12_HDR_MASK) == nodeid) {
                payload.vcc2++;
                
                if (rf12_len == 1) {
                    // a single byte loads the ramp from EEPROM
                    setDauer(rf12_data[0]);
                }
                if (RF12_WANTS_ACK)	
                    rf12_sendStart(RF12_ACK_REPLY, 0, 0);
                Sleepy::loseSomeTime(100);  // eine 10tel sekunde 
                sendPayload();  // minütlicher Report
                done = true;
                
         } 
         if(antwortTimer-- == 0) done = true;
    } 
 
    rf12_sleep(RF12_SLEEP);
    
    
    
    zehntelsekundenTimer = 600;  // eine Minute
  }//else    

  //if (Serial.available())
  //  handleInput(Serial.read());


   
   

}
