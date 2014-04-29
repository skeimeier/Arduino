/*
 * Arduino ENC28J60 EtherCard NTP client demo
 * With extra bits for nanode
 */

#define DEBUG

#include <avr/pgmspace.h>
#include <EtherCard.h>
#include <Time.h> 
#include <PortsLCD.h>

// Jan 1 
#define SECS_YR_1900_2000  (3155673600UL)


//static uint8_t mymac[6] = { 0x54,0x55,0x58,0x10,0x00,0x25};
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x32 };

// IP and netmask allocated by DHCP
static uint8_t myip[4] = { 0,0,0,0 };
static uint8_t mynetmask[4] = { 0,0,0,0 };
static uint8_t gwip[4] = { 0,0,0,0 };
static uint8_t dnsip[4] = { 0,0,0,0 };
static uint8_t dhcpsvrip[4] = { 0,0,0,0 };

// Timeserver Braunschweig 
static uint8_t pbt_1[4] = { 192,53,103,108 };
const int timeZone = 1;     // Central European Time
int daylightSaving = 1;

// Find list of servers at http://support.ntp.org/bin/view/Servers/StratumTwoTimeServers
// Please observe server restrictions with regard to access to these servers.
// This number should match how many ntp time server strings we have

// Packet buffer, must be big enough to packet and payload
#define BUFFER_SIZE 550
byte Ethernet::buffer[BUFFER_SIZE];
uint8_t NTPclientPort = 123;


// The next part is to deal with converting time received from NTP servers
// to a value that can be displayed. This code was taken from somewhere that
// I cant remember. Apologies for no acknowledgement.

uint32_t prevDisplay = 0;
uint32_t timeLong;
uint32_t lastUpdate = 0;

// Number of seconds between 1-Jan-1900 and 1-Jan-1970, unix time starts 1970
// and ntp time starts 1900.
#define GETTIMEOFDAY_TO_NTP_OFFSET 2208988800UL

#define	EPOCH_YR	1970
//(24L * 60L * 60L)
#define	SECS_DAY	86400UL  
#define	LEAPYEAR(year)	(!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define	YEARSIZE(year)	(LEAPYEAR(year) ? 366 : 365)

static const char day_abbrev[] PROGMEM = "SunMonTueWedThuFriSat";




void setup(){
  Serial.begin(19200);
  Serial.println( F("[EtherCard NTP Client]" ) );
  

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
  uint16_t dat_p;
   int plen = 0;
 
   if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      digitalClockDisplay();  
    }
  }else{
 
  
  }

  
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
                      timeLong += (timeZone + daylightSaving) * SECS_PER_HOUR;
                      setTime(timeLong);
                  }
              }
     
      
    }
     // Request an update every 30s
    if( lastUpdate + 30000L < millis() ) {
      // time to send request
       lastUpdate = millis();
       
        ether.printIp("SRV: ", pbt_1);
        
        Serial.print( F("Send NTP request " ));
      
        ether.ntpRequest(pbt_1, NTPclientPort);
        Serial.print( F("clientPort: "));
        Serial.println(NTPclientPort, DEC );
     }
   

}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits){
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}



time_t getNtpTime()
{
  
 /*
  uint16_t dat_p;
   int plen = 0;

*/
        ether.printIp("Time_SRV: ", pbt_1);
        
        Serial.print( F("Send NTP request " ));

      
        //ether.ntpRequest(ether.hisip, clientPort);
        ether.ntpRequest(pbt_1, NTPclientPort);
        Serial.print( F("clientPort: "));
        Serial.println(NTPclientPort, DEC );
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
