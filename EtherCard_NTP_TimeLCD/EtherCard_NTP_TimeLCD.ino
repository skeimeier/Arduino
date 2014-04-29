/*
 * Arduino ENC28J60 EtherCard NTP client demo
 * With extra bits for nanode
 */

#define DEBUG 1
#define SERIAL 1

#include <avr/pgmspace.h>
#include <EtherCard.h>
#include <Time.h> 
#include <PortsLCD.h>


PortI2C myI2C (1);
LiquidCrystalMyI2C lcd (myI2C);


//static uint8_t mymac[6] = { 0x54,0x55,0x58,0x10,0x00,0x25};
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x32 };


// Timeserver Braunschweig 
static uint8_t pbt_1[4] = { 192,53,103,108 };
const int timeZone = 1;     // Central European Time
int daylightSaving = 1;

// Find list of servers at http://support.ntp.org/bin/view/Servers/StratumTwoTimeServers
// Please observe server restrictions with regard to access to these servers.
// This number should match how many ntp time server strings we have

// Packet buffer, must be big enough to packet and payload
#define BUFFER_SIZE 1000
byte Ethernet::buffer[BUFFER_SIZE];
uint8_t NTPclientPort = 123;
static BufferFiller bfill;  // used as cursor while filling the buffer


// The next part is to deal with converting time received from NTP servers
// to a value that can be displayed. This code was taken from somewhere that
// I cant remember. Apologies for no acknowledgement.

uint32_t prevDisplay = 0;
uint32_t timeLong;
uint32_t lastUpdate = 0;

// Number of seconds between 1-Jan-1900 and 1-Jan-1970, unix time starts 1970
// and ntp time starts 1900.
#define GETTIMEOFDAY_TO_NTP_OFFSET 2208988800UL


static const char day_abbrev[] PROGMEM = "SunMonTueWedThuFriSat";


#if DEBUG
static int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
#endif


//// HTTP  Sachen
char okHeader[] PROGMEM = 
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n"
;

static void homePage(BufferFiller& buf) {
    int h = hour();
    int m = minute();
    int s = second();  
    int d = day();
    int mm = month();
    int y = year();
    
    char dstr[4];
    uint8_t i;
    // build weekday as string 
    i=0;
    while (i<3){
          dstr[i]= pgm_read_byte(&(day_abbrev[(weekday()-1)*3 + i])); 
          i++;
     }
      dstr[3]='\0';
    
    buf.emit_p(PSTR("$F\r\n"
        "<meta http-equiv='refresh' content='10'/>"
        "<title>StromEtherNode_32</title>" 
        "StromEtherNode_32 "
            "- <a href='c'>configure</a> - <a href='s'>send packet</a>"
        "<h3>NTP TIME:</h3>"
        "<P>$D$D:$D$D:$D$D</P>"
        "<P>$S $D.$D.$D</P>"), okHeader, h/10, h%10, m/10, m%10, s/10, s%10,
        dstr,d,mm,y);
     long t = millis() / 1000;
    word u_h = t / 3600;
    byte u_m = (t / 60) % 60;
    byte u_s = t % 60;
    buf.emit_p(PSTR(
        "</pre>"
        "Uptime is $D$D:$D$D:$D$D"), u_h/10, u_h%10, u_m/10, u_m%10, u_s/10, u_s%10);
#if DEBUG
    buf.emit_p(PSTR(" ($D bytes free)"), freeRam());
#endif
}

void setup(){
  Serial.begin(19200);
  Serial.println( F("[EtherCard NTP Client]" ) );
  
  delay(500);
  lcd.begin(16,2);
  // Print a message to the LCD.
  lcd.print(F("[Eth NTP Client]" ));

  uint8_t rev = ether.begin(sizeof Ethernet::buffer, mymac);
  Serial.print( F("\nENC28J60 Revision ") );
  Serial.println( rev, DEC );
  if ( rev == 0) 
    Serial.println( F( "Failed to access Ethernet controller" ) );

  Serial.println( F( "Setting up DHCP" ));
  if (!ether.dhcpSetup())
    Serial.println( F( "DHCP failed" ));
  
  ether.printIp("My IP: ", ether.myip);
  //ether.printIp("Netmask: ", ether.mymask);
  ether.printIp("GW IP: ", ether.gwip);
  ether.printIp("DNS IP: ", ether.dnsip);

  lastUpdate = millis()-25000U;

  prevDisplay = now();
  //setSyncProvider(getNtpTime);
   
}

void loop(){
  uint16_t pos;
   int len = 0;
 
  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      digitalClockDisplay();  
    }
  }else{
 
  
  }

  
  // Main processing loop now we have our addresses
    // handle ping and wait for a tcp packet
    len = ether.packetReceive();
    pos=ether.packetLoop(len);
    // Has unprocessed packet response
    if (len > 0) {

          // process NTP request
              timeLong = 0L;  
              if (ether.ntpProcessAnswer(&timeLong,NTPclientPort)) {
                  Serial.print( F( "Time:" ));
                  Serial.println(timeLong); // secs since year 1900      
                  if (timeLong) {
                      timeLong -= GETTIMEOFDAY_TO_NTP_OFFSET;
                      timeLong += (timeZone + daylightSaving) * SECS_PER_HOUR;
                      setTime(timeLong);
                  }
              }
            
  
      
    }
    // process HTTP
    if(pos){
          bfill = ether.tcpOffset();
          char* data = (char *) Ethernet::buffer + pos;
#if DEBUG
          Serial.println(data);
#endif
         // receive buf hasn't been clobbered by reply yet
         if (strncmp("GET / ", data, 6) == 0)
              homePage(bfill);
              //else if (strncmp("GET /c", data, 6) == 0)
              //    configPage(data, bfill);
              //else if (strncmp("GET /s", data, 6) == 0)
              //    sendPage(data, bfill);
              else{
              }//else
              ether.httpServerReply(bfill.position()); // send web page data
    }
    
    
     // Request an update every 30s
    if( lastUpdate + 30000L < millis() ) {
      // time to send request
       lastUpdate = millis();
       
        ether.printIp("SRV: ", pbt_1);
#if DEBUG        
        
        Serial.print( F("Send NTP request " ));
#endif
     
        ether.ntpRequest(pbt_1, NTPclientPort);
#if DEBUG        
        Serial.print( F("clientPort: "));
        Serial.println(NTPclientPort, DEC );
#endif
    }
   

}

void digitalClockDisplay(){
  // digital clock display of the time
  char dstr[4];
  uint8_t i;
  // build weekday as string 
  i=0;
  while (i<3){
    dstr[i]= pgm_read_byte(&(day_abbrev[(weekday()-1)*3 + i])); 
    i++;
  }
  dstr[3]='\0';

  lcd.clear();
  lcd.print(hour());
  printDigits(minute());
  printDigits(second());
  lcd.setCursor(0,1);
  lcd.print((char *)dstr);
  lcd.print(" ");
  lcd.print(day());
  lcd.print(".");
  lcd.print(month());
  lcd.print(".");
  lcd.print(year()); 
 
}

void printDigits(int digits){
  // utility for digital clock display: prints preceding colon and leading 0
  lcd.print(":");
  if(digits < 10)
    lcd.print('0');
  lcd.print(digits);
}



time_t getNtpTime()
{
  
 /*
  uint16_t dat_p;
   int plen = 0;

*/
        ether.printIp("Time_SRV: ", pbt_1);
#if DEBUG        
        Serial.print( F("Send NTP request " ));
#endif
      
        //ether.ntpRequest(ether.hisip, clientPort);
        ether.ntpRequest(pbt_1, NTPclientPort);
#if DEBUG        
        Serial.print( F("clientPort: "));
        Serial.println(NTPclientPort, DEC );
#endif
   /*    
        uint32_t beginWait = millis();
        while (millis() - beginWait < 1500) {
          // Main processing loop now we have our addresses
          // handle ping and wait for a tcp packet
          plen = ether.packetReceive();
          dat_p=ether.packetLoop(plen);
          // Has unprocessed packet response
          if (plen > 0) {
              timeLong = 0L;  
              if (ether.ntpProcessAnswer(&timeLong,NTPclientPort)) {
                  Serial.print( F( "Time:" ));
                  Serial.println(timeLong); // secs since year 1900      
                  if (timeLong) {
                      timeLong -= GETTIMEOFDAY_TO_NTP_OFFSET;
                       return timeLong + timeZone * SECS_PER_HOUR;
                  }
              }
          }
        }//While
   Serial.println("No NTP Response :-(");
   */
   return timeLong;  // return 0 if unable to get the time

  
}
