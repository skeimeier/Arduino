// Send out a radio packet every minute, consuming as little power as possible
// 2012-05-09 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#include <JeeLib.h>
#include <avr/sleep.h>

#define BLIP_NODE 12 // wireless node ID to use for sending blips
#define BLIP_GRP 19 // wireless net group !! hier über Relay-Node 
#define BLIP_ID 1 // set this to a unique ID to disambiguate multiple nodes
#define SEND_MODE 2 // set to 3 if fuses are e=06/h=DE/l=CE, else set to 2

struct {
  byte id; // identity, should be different for each node
  long ping; // 32-bit counter
  byte vcc1; // VCC before transmit, 1.0V = 0 .. 6.0V = 250
  byte vcc2; // VCC after transmit, will be sent in next cycle
  byte tuerstatus; //Tuer geöffnet = 1 / geschlossen = 0
  long tuercounter;  //Zählt Öffnungen + schliessungen
} payload;

volatile int seconds  = 0;          // send status every 60 seconds
volatile int bounceTime  = 20;       // Switch bouncing time in milliseconds

volatile int counter1    = 0;        // Counter incremented by pin change interrupt
volatile unsigned long IRQ1PrevTime; // Last time in milliseconds IRQ1 arrived
volatile int IRQ1PrevVal   = 0;      // Contact level last IRQ1
volatile int irqFlag1 = 0;           // 1=display counter after IRQ1; 0=do nothing

volatile int counter2    = 0;        // Counter incremented by pin change interrupt
volatile unsigned long IRQ2PrevTime; // Last time in milliseconds IRQ1 arrived
volatile int IRQ2PrevVal   = 0;      // Contact level last IRQ1
volatile int irqFlag2 = 0;           // 1=display counter after IRQ1; 0=do nothing


volatile bool adcDone;

// for low-noise/-power ADC readouts, we'll use ADC completion interrupts
ISR(ADC_vect) { adcDone = true; }

// this must be defined since we're using the watchdog for low-power waiting
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

ISR(PCINT2_vect)
{
  // You have to write your own interrupt handler. Don't change the name!
  // This code will be called anytime when PCINT23 switches high to low, 
  // or low to high.
  byte PVal;                                   // Port value (8 Bits)
  byte IRQ1ActVal;                             // Actual IRQ1 value
  long unsigned IRQ1ActTime;

  PVal = PIND;                                 // Read port D (8 bit)
  IRQ1ActVal = PVal & (1<<PCINT20);            // Mask out all except IRQ1
  IRQ1ActVal = IRQ1ActVal >> PCINT20;          // shift to right for bit0 position
  IRQ1ActTime=millis();                        // Read actual millis time
  if(IRQ1ActTime - IRQ1PrevTime > bounceTime)  // No bouncing anymore:
  {  
    // No contact bouncing anymore:
    if(IRQ1PrevVal==0 && IRQ1ActVal==1)        // Transition 0-->1
    {
      // Place your command for rising signal here...
      counter1++;
      if(counter1>255) counter1 = 0;
      IRQ1PrevTime=IRQ1ActTime;  
      IRQ1PrevVal=IRQ1ActVal;   
      irqFlag1=1;
    }
    if(IRQ1PrevVal==1 && IRQ1ActVal==0)        // Transition 1-->0
    {
      // Place your command for falling signal here... 
      IRQ1PrevVal=IRQ1ActVal;
    }
  }
}
ISR(PCINT1_vect)
{
  // You have to write your own interrupt handler. Don't change the name!
  // This code will be called anytime when PCINT23 switches high to low, 
  // or low to high.
  byte PVal;                                   // Port value (8 Bits)
  byte IRQ2ActVal;                             // Actual IRQ1 value
  long unsigned IRQ2ActTime;

  PVal = PINC;                                 // Read port C (8 bit)
  IRQ2ActVal = PVal & (1<<PCINT8);            // Mask out all except IRQ1
  IRQ2ActVal = IRQ2ActVal >> PCINT8;          // shift to right for bit0 position
  IRQ2ActTime=millis();                        // Read actual millis time
//  if(IRQ2ActTime - IRQ2PrevTime > bounceTime)  // No bouncing anymore:
//  {  
    // No contact bouncing anymore:
    if(IRQ2PrevVal==0 && IRQ2ActVal==1)        // Transition 0-->1  = Tür zu
    {
      // Place your command for rising signal here...
      payload.tuerstatus = 0;
      IRQ2PrevTime=IRQ2ActTime;  
      IRQ2PrevVal=IRQ2ActVal;   
      irqFlag2=1;
    }
    if(IRQ2PrevVal==1 && IRQ2ActVal==0)        // Transition 1-->0  = Tür Auf
    {
      // Place your command for falling signal here... 
      payload.tuerstatus = 1;
      IRQ2PrevTime=IRQ2ActTime;  
      IRQ2PrevVal=IRQ2ActVal;
      irqFlag2=1;
   }
 // }
}


static byte vccRead (byte count =4) {
  set_sleep_mode(SLEEP_MODE_ADC);
  ADMUX = bit(REFS0) | 14; // use VCC as AREF and internal bandgap as input
  bitSet(ADCSRA, ADIE);
  while (count-- > 0) {
    adcDone = false;
    while (!adcDone)
      sleep_mode();
  }
  bitClear(ADCSRA, ADIE);
  // convert ADC readings to fit in one byte, i.e. 20 mV steps:
  // 1.0V = 0, 1.8V = 40, 3.3V = 115, 5.0V = 200, 6.0V = 250
  return (55U * 1024U) / (ADC + 1) - 50;
}

void setup() {
  Serial.begin(57600);
  Serial.println("\n[Terassentuer]");
  delay(2000);
  cli();
  CLKPR = bit(CLKPCE);
#if defined(__AVR_ATtiny84__)
  CLKPR = 0; // div 1, i.e. speed up to 8 MHz
#else
  CLKPR = 1; // div 2, i.e. slow down to 8 MHz
#endif
  sei();
  
   pinMode(4, INPUT);
   digitalWrite(4, HIGH);
   pinMode(14, INPUT);
   digitalWrite(14, HIGH);
  // This is ATMEGA368 specific, see page 75 of long datasheet
  // PCICR: Pin Change Interrupt Control Register - enables interrupt vectors
  // Bit 2 = enable PC vector 2 (PCINT23..16)
  // Bit 1 = enable PC vector 1 (PCINT14..8)
  // Bit 0 = enable PC vector 0 (PCINT7..0)
    PCICR |= (1 << PCIE2);             // Set port bit in CICR
    PCICR |= (1 << PCIE1);             // Set port bit in CICR
  // Pin change mask registers decide which pins are enabled as triggers:
    PCMSK2 |= (1<<PCINT20);            // Set pin interrupt for digital input
    PCMSK1 |= (1<<PCINT8);            // Set pin interrupt for digital input
    IRQ1PrevTime=millis();             // Hold actual time
    IRQ2PrevTime=millis();             // Hold actual time
    interrupts();                      // Enable interrupts

    //rf12_config();
  rf12_initialize(BLIP_NODE, RF12_868MHZ, BLIP_GRP);
  // see http://tools.jeelabs.org/rfm12b
  rf12_control(0xC040); // set low-battery level to 2.2V i.s.o. 3.1V
  rf12_sleep(RF12_SLEEP);

  payload.id = BLIP_ID;
}

static byte sendPayload () {

  rf12_sleep(RF12_WAKEUP);
  while (!rf12_canSend())
    rf12_recvDone();
  rf12_sendStart(0, &payload, sizeof payload);
  rf12_sendWait(SEND_MODE);
  rf12_sleep(RF12_SLEEP);
}

// This code tries to implement a good survival strategy: when power is low,
// don't transmit - when power is even lower, don't read out the VCC level.
//
// With a 100 µF cap, normal packet sends can cause VCC to drop by some 0.6V,
// hence the choices below: sending at >= 2.7V should be ok most of the time.

#define VCC_OK 85 // >= 2.7V - enough power for normal 1-minute sends
#define VCC_LOW 80 // >= 2.6V - sleep for 1 minute, then try again
#define VCC_DOZE 75 // >= 2.5V - sleep for 5 minutes, then try again
                      // < 2.5V - sleep for 60 minutes, then try again
#define VCC_SLEEP_MINS(x) ((x) >= VCC_LOW ? 1 : (x) >= VCC_DOZE ? 5 : 60)

// Reasoning is that when we're about to try sending and find out that VCC
// is far too low, then let's just send anyway, as one final sign of life.

#define VCC_FINAL 70 // <= 2.4V - send anyway, might be our last swan song

void loop() {
  byte vcc = payload.vcc1 = vccRead();
  
  if (irqFlag1==1) {                  // Flag was set by IRQ1 routine
      //Serial.write("IRQ-A gruen rising edge, Counter1 = ");
      sendPayload();
     vcc = payload.vcc2 = vccRead(); // measure and remember the VCC drop
      seconds = 60;
      irqFlag1=0;                      // Reset IRQ1 flag
  }
  if (irqFlag2==1){                   // Flag was set by IRQ1 routine
       payload.tuercounter++;
     sendPayload();
      vcc = payload.vcc2 = vccRead(); // measure and remember the VCC drop
      seconds = 60;
      irqFlag2=0;                      // Reset IRQ1 flag
 
      /*Serial.print("Tuerstatus: ");
      if(payload.tuerstatus == 0) Serial.println("ZU ");
      if(payload.tuerstatus == 1) Serial.println("AUF");
      Serial.print("Tuercounter: ");
      Serial.println((int)payload.tuercounter);
     */
     
   }
 

  //byte minutes = VCC_SLEEP_MINS(vcc);
  if(seconds -- > 0){
    Sleepy::loseSomeTime(1000);
  }else{
    ++payload.ping;
    sendPayload();
    vcc = payload.vcc2 = vccRead(); // measure and remember the VCC drop
    seconds = 60;
  }    
}
