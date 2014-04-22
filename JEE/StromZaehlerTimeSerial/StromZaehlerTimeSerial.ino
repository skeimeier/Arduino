/* 
 * TimeSerial.pde
 * example code illustrating Time library set through serial port messages.
 *
 * Messages consist of the letter T followed by ten digit time (as seconds since Jan 1 1970)
 * you can send the text on the next line using Serial Monitor to set the clock to noon Jan 1 2013
 T1357041600  
 *
 * A Processing example sketch to automatically send the messages is inclided in the download
 * On Linux, you can use "date +T%s > /dev/ttyACM0" (UTC time zone)
 */ 
 
#include <Time.h>  

#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

int oldVal = LOW;
int newVal;
unsigned long lastchange;
unsigned long lastscheibe=0;
unsigned long dauer;
unsigned long dauerscheibe;
unsigned long counter;

unsigned long oldMillis;
unsigned long newMillis;

void setup()  {
  Serial.begin(9600);
  while (!Serial) ; // Needed for Leonardo only
  pinMode(13, OUTPUT);
  pinMode(2,INPUT);
  setSyncProvider( requestSync);  //set function to call when sync required
  Serial.println("Waiting for sync message");
}

void loop(){    
  if (Serial.available()) {
    processSyncMessage();
    lastchange = now();
  }
  if (timeStatus()!= timeNotSet) {
  
      newVal = digitalRead(2);
      digitalWrite(13, newVal);
   
      if (newVal != oldVal) {
          oldVal = newVal; 
          dauer = millis() - oldMillis;
          oldMillis = millis();
          if(dauer > 200){
            dauerscheibe = now() - lastscheibe ;
            lastscheibe = now();
            digitalWrite(13, !newVal);
            Serial.print(newVal);
            Serial.print(" ");
            Serial.print(dauer );
            Serial.print(" millis ");
            Serial.print(37500/dauerscheibe);
            Serial.print(" Watt ");
            Serial.print( ++counter);
            Serial.print(" Ticks ");
            
            digitalClockDisplay();
            
          }  
    
       } 
  }
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println(); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
       setTime(pctime); // Sync Arduino clock to the time received on the serial port
        digitalClockDisplay();
        oldMillis = millis();

     }
  }
}

time_t requestSync()
{
  Serial.write(TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}

