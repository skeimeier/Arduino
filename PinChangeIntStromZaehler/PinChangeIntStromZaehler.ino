// PinChangeIntExample, version 1.1 Sun Jan 15 06:24:19 CST 2012
// See the Wiki at http://code.google.com/p/arduino-pinchangeint/wiki for more information.
//-------- define these in your sketch, if applicable ----------------------------------------------------------
// You can reduce the memory footprint of this handler by declaring that there will be no pin change interrupts
// on any one or two of the three ports.  If only a single port remains, the handler will be declared inline
// reducing the size and latency of the handler.
#define NO_PORTB_PINCHANGES // to indicate that port b will not be used for pin change interrupts
#define NO_PORTC_PINCHANGES // to indicate that port c will not be used for pin change interrupts
// #define NO_PORTD_PINCHANGES // to indicate that port d will not be used for pin change interrupts
// if there is only one PCInt vector in use the code can be inlined
// reducing latency and code size
// define DISABLE_PCINT_MULTI_SERVICE below to limit the handler to servicing a single interrupt per invocation.
// #define       DISABLE_PCINT_MULTI_SERVICE
//-------- define the above in your sketch, if applicable ------------------------------------------------------
#include <PinChangeInt.h>
#include <EEPROM.h>


// This example demonstrates a configuration of 3 interrupting pins and 2 interrupt functions.
// All interrupts are serviced immediately, but one of the pins (pin 4) will show you immediately
// on the Terminal.  The other function connected to 2 pins sets an array member that is queried in loop().
// You can then query the array at your leisure.
// This makes loop timing non-critical.

// Add more Pins at your leisure.
// For the Analog Input pins used as digital input pins, and you can use 14, 15, 16, etc.
// or you can use A0, A1, A2, etc. (the Arduino code comes with #define's
// for the Analog Input pins and will properly recognize e.g., pinMode(A0, INPUT);
#define PIN1 2
#define LED 13
//#define PIN3 4


unsigned long lastminute;
boolean once = true;


unsigned long interrupt_count=0, runden=0; 
unsigned long lastrundemillis=0, lastmillis, gap, rundenzeit=0 ;
unsigned long schnitt;
uint8_t count;

static short n = 1;          //Zählvariable anlegen
static unsigned long avgsum;      //Variable zur Summenspeicherung anlegen


void quicfunc() {
  interrupt_count++;
  count++;
  gap = millis() - lastmillis;
  lastmillis = millis();
};


void setup() {
  pinMode(PIN1, INPUT); digitalWrite(PIN1, HIGH);
  pinMode(LED,OUTPUT);
  PCintPort::attachInterrupt(PIN1, &quicfunc, FALLING);  // add more attachInterrupt code as required
  Serial.begin(115200);
  Serial.println("---------------------------------------");
  count = 0;
  lastmillis = millis();
  lastrundemillis = millis();
   avgsum = 0;
     lastminute = millis();
     gap = 0;
}

uint8_t i;
void loop() {
  //Serial.print(".");
  //delay(1000);
  digitalWrite(LED,digitalRead(PIN1));
  if(interrupt_count < 38000){
    if (count != 0) {
      if(interrupt_count > 1){
         schnitt = mittelwert_filter(gap);
      }
      if( ( millis()-lastminute) > 60000){
            lastminute = millis();
           // calculate from slidung average
        Serial.print(" Mittel ");
        Serial.print(37500000/(schnitt*380));
        Serial.println(" Watt ");

      }
      /*
      Serial.print("Count ");
      Serial.print(interrupt_count, DEC);
      Serial.print(" Gap ");
      Serial.print(gap, DEC);
      Serial.print(" Avg ");
      Serial.print(schnitt, DEC);
      Serial.println();
      */      
     
      count = 0;
      if( (interrupt_count > 1) && (gap > (5*schnitt)) ){
        rundenzeit = millis()- lastrundemillis;
        lastrundemillis = millis();
        runden++;
        Serial.print(">>> Runden ");
        Serial.print(runden, DEC);
        Serial.print(" Ticks ");
        Serial.print(interrupt_count, DEC);
         Serial.print(" Dauer ");
        Serial.print(rundenzeit, DEC);
        Serial.print(" mit ");
        Serial.print(37500000/rundenzeit);
        Serial.print(" Watt ");
        interrupt_count = 0;
        n = 1;          //Zählvariable anlegen
        avgsum=0;      //Variable zur Summenspeicherung anlegen

        Serial.println();
      }
    }
  }
}

unsigned long mittelwert_filter(long newval)
{
    unsigned long avg;          //Variable zur Mittelwertspeicherung anlegen
   
   if (n<16)               //Mit dieser if Schleife wird die Fenstrgröße festgelegt
                       //hier sollte eine 2er potenz für die Größe gewählt werden.
   {
      avgsum += newval;          //Wert n-mal aufaddieren
      avg = avgsum/n;          //Summer durch n teilen
      n++;                //n um eins erhöhen
   }
   else 
   {
      avgsum -= avgsum/16;
      avgsum += newval;
      avg = avgsum/16;
   }
   return avg;

}
