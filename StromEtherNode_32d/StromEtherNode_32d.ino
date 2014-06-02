// Arduino demo sketch for testing RFM12B + ethernet
// 2010-05-20 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

// Listens for RF12 messages and displays valid messages on a webpage
// Memory usage exceeds 1K, so use Atmega328 or decrease history/buffers
//
// This sketch is derived from RF12eth.pde:
// May 2010, Andras Tucsni, http://opensource.org/licenses/mit-license.php

#define RF69_COMPAT 0
 
#include <EtherCard.h>
#include <JeeLib.h>
#include <avr/eeprom.h>
#include <PortsLCD.h>


PortI2C myI2C (1);
LiquidCrystalMyI2C lcd (myI2C);

#define PC_INT_PIN    5   // pinchangeInterupt an Arduiono pin5 = JEE PORT2 DIO = AVR PD5
#define boardLed 9

#define DEBUG   1   // set to 1 to display free RAM on web page
#define SERIAL  0   // set to 1 to show incoming requests on serial port

#define CONFIG_EEPROM_ADDR ((byte*) 0x10)

// configuration, as stored in EEPROM
struct Config {
    byte rounds_per_kwh;               // Runden für 1 kWh des Ferrariszählers
    unsigned long total_rounds;        // gepeicherter Zwischenstand 
    word update_page;                      // HTTP Refresh alle n Sekunden
    word port;                        // UDP Port to send to
    byte valid; // keep this as last byte
} config;

// ethernet interface mac address - must be unique on your network
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x32 };


static BufferFiller bfill;  // used as cursor while filling the buffer

byte Ethernet::buffer[800];   // tcp/ip send and receive buffer

static byte collBuf [200], collPos;  // this buffer will be used to construct a collectd UDP packet

static void loadConfig() {
    for (byte i = 0; i < sizeof config; ++i)
        ((byte*) &config)[i] = eeprom_read_byte(CONFIG_EEPROM_ADDR + i);
    if (config.valid != 251) {
        config.valid = 251;
        config.rounds_per_kwh = 96;               // Runden für 1 kWh des Ferrariszählers
        config.total_rounds = 1198177 ;         // gepeicherter Zwischenstand bei Ersteinrichtung
        config.update_page = 5;
        config.port = 5005;
    }
}

static void saveConfig() {
    for (byte i = 0; i < sizeof config; ++i)
        eeprom_write_byte(CONFIG_EEPROM_ADDR + i, ((byte*) &config)[i]);
}

#if DEBUG
static int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
#endif

////////////////////////
/// STRONMZÄGHLER Sachen
////////////////////

unsigned long lastupdate;
boolean once = true;


static unsigned long last_int_count, interrupt_count=0, runden=0; 
static unsigned long lastrundemillis=0, lastmillis, gap, rundenzeit=0 ;
static unsigned long schnitt, watt_last_round, watt_mittel;
boolean  has_int;

static short n = 1;          //Zählvariable anlegen
static unsigned long avgsum;      //Variable zur Summenspeicherung anlegen

ISR(PCINT2_vect) { 
    if (!bitRead(PIND, PC_INT_PIN)){ //Falling Edge
      interrupt_count++;
      has_int = true;
      gap = millis() - lastmillis;
      lastmillis = millis();
    }
}




void initStromZaehler(){
  pinMode(boardLed,OUTPUT);

  pinMode(PC_INT_PIN, INPUT); 
  digitalWrite(PC_INT_PIN, HIGH);
  bitSet(PCMSK2, PC_INT_PIN);      // pin-change on Port D
  bitSet(PCICR, PCIE2);         // enable


  
  has_int = false;
  avgsum = 0;
  gap = 0;
  n = 1;
  lastrundemillis = millis();
  lastupdate = millis();
  runden = config.total_rounds;
  lastmillis = millis();
}

void doStromCount(){
  digitalWrite(boardLed,digitalRead(PC_INT_PIN));

  if (has_int) {
      if(interrupt_count >= 1){
        //schnitt = mittelwert_filter(gap);        
        if( (n >= 16) && (gap > 5*schnitt) ){
            rundenzeit = millis()- lastrundemillis;
            lastrundemillis = millis();
            runden++;
            watt_last_round = 37500000L/rundenzeit;
            lcd.setCursor(0,3);
            lcd.print("Total: ");
            lcd.print(runden/config.rounds_per_kwh);
            lcd.print(".");
            lcd.print(((runden%config.rounds_per_kwh)*100)/config.rounds_per_kwh);
           /* lcd.setCursor(0,1);
            lcd.print("G:");
            lcd.print(gap);
            lcd.print(" S:");
            lcd.print(schnitt);
            lcd.print("     ");
            */ 
            last_int_count = interrupt_count;
           // n = 1;          //Zählvariable anlegen
           // avgsum=0;      //Variable zur Summenspeicherung anlegen
            forwardToUDP();
           
            interrupt_count = 0;
          }else{
             schnitt = mittelwert_filter(gap);
          }
      }
      
      if( ( millis()-lastupdate) > (config.update_page * 1000L)){ // alle n sekunden
            lastupdate = millis();
            watt_mittel = 37500000L/(schnitt*380);
           // calculate from sliding average
             lcd.setCursor(0,2);
             lcd.print("Jetzt: ");
             lcd.print(watt_mittel);
             lcd.print(" W       ");
             forwardToUDP();

      }
      has_int = false;
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
/// ENDE STRONMZÄGHLER Sachen
////////////////////


/////////////////////////
//// UDP 
static void collectStr (const char* data) {
  word len = strlen(data);
  //collectTypeLen(type, len);
  strcpy((char*) collBuf + collPos, data);
  collPos += len;
}

static void collectPayload () {
  char buf[15];
  // Copy Stromzähler counts to udp
    collectStr(" R:");
    sprintf(buf,"%lu", runden);   
    collectStr(buf);
    collectStr(" T:");
    sprintf(buf,"%lu", last_int_count);  //Ticks 
    collectStr(buf);
    collectStr(" W:");
    sprintf(buf,"%lu", watt_mittel);  //aktuelle Leistung Watt 
    collectStr(buf);
    collectStr(" V:");
    sprintf(buf,"%lu", runden/config.rounds_per_kwh);  //Verbrauch
    collectStr(buf);
    collectStr(".");
    sprintf(buf,"%02d", (int)(((runden%config.rounds_per_kwh)*100)/config.rounds_per_kwh));  //nachkomma
    collectStr(buf);
}

static void forwardToUDP () {
  static byte destIp[] = { 255,255,255,255 }; // UDP multicast address
    
  collPos = 0;
  collectStr("[StromEtherNode_32] ");
  collectPayload();
  
  ether.sendUdp ((char*) collBuf, collPos, 23456, destIp, config.port);
}



//// ENDE UDP
////////////////////////


void setup(){
#if SERIAL
    Serial.begin(57600);
    Serial.println(F("\n[StromEtherNode_32]"));
#endif
    loadConfig();
  lcd.begin(20,4);
  lcd.set4Lines();
  
  // Print a message to the LCD.
  lcd.print(F("[Ether Strom]" ));
    
    if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
      Serial.println(F( "Failed to access Ethernet controller"));
    if (!ether.dhcpSetup())
      Serial.println(F("DHCP failed"));
#if SERIAL
    ether.printIp("IP: ", ether.myip);
#endif
    lcd.setCursor(0,1);
    lcd.print("IP: ");
    lcd.print(ether.myip[0]);
    lcd.print(".");
    lcd.print(ether.myip[1]);
    lcd.print(".");
    lcd.print(ether.myip[2]);
    lcd.print(".");
    lcd.print(ether.myip[3]);
    delay(5000);
   // INIT für Stromzähler
    initStromZaehler();

}

char okHeader[] PROGMEM = 
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n"
;

static void homePage(BufferFiller& buf) {
    buf.emit_p(PSTR("$F\r\n"
        "<meta http-equiv='refresh' content='$D'/>"
        "<title>StromEtherNode_32</title>" 
        "<h1>StromEtherNode_32</h1>"
        "<hr>"
        "<h3>Zaehler:</h3>"
        "<P>Verbrauch aktuell: $L Watt<br>"
        "Runden: $L <br> Ticks: $L<br> Rundenzeit: $L ms <br>"
        "Verbrauch letzte Runde: $L Watt</P>"
        "<hr>"
        "<h3>Verbrauch Total: $L.$L kWh</h3>"
), okHeader,config.update_page, watt_mittel, runden, last_int_count, rundenzeit, watt_last_round, 
          runden/config.rounds_per_kwh,((runden%config.rounds_per_kwh)*100)/config.rounds_per_kwh);
    long t = millis() / 1000;
    word h = t / 3600;
    byte m = (t / 60) % 60;
    byte s = t % 60;
    buf.emit_p(PSTR(
        "<hr>"
         "<a href='c'>configure</a>"
    
        "</pre>"
        "<P>Uptime is $D$D:$D$D:$D$D"), h/10, h%10, m/10, m%10, s/10, s%10);
#if DEBUG
    buf.emit_p(PSTR(" ($D bytes free)</P>"), freeRam());
#endif
}


static int getIntArg(const char* data, const char* key, int value =-1) {
    char temp[10];
    if (ether.findKeyVal(data + 7, temp, sizeof temp, key) > 0)
        value = atoi(temp);
    return value;
}

static unsigned long getLongArg(const char* data, const char* key, unsigned long value =-1) {
    char temp[10];
    if (ether.findKeyVal(data + 7, temp, sizeof temp, key) > 0)
        value = atol(temp);
    return value;
}

static void configPage(const char* data, BufferFiller& buf) {
    // pick up submitted data, if present
    if (data[6] == '?') {
        byte r = getIntArg(data, "r");
        word u = getIntArg(data, "u", 0);
        long t = getLongArg(data, "t");
        word p = getIntArg(data, "p");
        if (1 <= r && r <= 250 && 1 <= u && u <= 3600) {
            // store values as new settings
            config.rounds_per_kwh = r;
            config.total_rounds = (t*96)/10;
            config.update_page = u;
            config.port = p;
            saveConfig();
            // re-init RF12 driver
            loadConfig();
            initStromZaehler();

            // redirect to the home page
            buf.emit_p(PSTR(
                "HTTP/1.0 302 found\r\n"
                "Location: /\r\n"
                "\r\n"));
            return;
        }
    }
    // else show a configuration form
    buf.emit_p(PSTR("$F\r\n"
        "<h3>Server node configuration</h3>"
        "<form>"
          "<p>"
            "Runden pro 1 kWh <input type=text name=r value='$D' size=1> (gewoehnlich 96)<br>"
            "Updaterate fuer Homepage <input type=text name=u value='$D' size=4> (1..3600 seconds)<br>"
            "Zaehlerstand setzen <input type=text name=t value='$L' size=10> (6-7 Stellen, Komma weglassen)<br>"
            "UDP-Port No. setzen <input type=text name=p value='$D' size=6> (4-6 Stellen, default: 5005)"
         "</p>"
          "<input type=submit value=Set>"
        "</form>"), okHeader, 
                    config.rounds_per_kwh, 
                    config.update_page , 
                    runden*10/config.rounds_per_kwh,
                    config.port
      );
}

static void sendPage(const char* data, BufferFiller& buf) {
    // pick up submitted data, if present
    const char* p = strstr(data, "b=");
    byte d = getIntArg(data, "d");
    if (data[6] == '?' && p != 0 && 0 <= d && d <= 31) {
        // prepare to send data as soon as possible in loop()
       // outDest = d & RF12_HDR_MASK ? RF12_HDR_DST | d : 0;
      /*  outCount = 0;
        // convert the input string to a number of decimal data bytes in outBuf
        ++p;
        while (*p != 0 && *p != '&') {
            outBuf[outCount] = 0;
            while ('0' <= *++p && *p <= '9')
                outBuf[outCount] = 10 * outBuf[outCount] + (*p - '0');
            ++outCount;
        }
#if SERIAL
        Serial.print("Send to ");
        Serial.print(outDest, DEC);
        Serial.print(':');
        for (byte i = 0; i < outCount; ++i) {
            Serial.print(' ');
            Serial.print(outBuf[i], DEC);
        }
        Serial.println();
#endif
        // redirect to home page
        buf.emit_p(PSTR(
            "HTTP/1.0 302 found\r\n"
            "Location: /\r\n"
            "\r\n"));
        return;
  */  
  }
    
    // else show a send form
    buf.emit_p(PSTR("$F\r\n"
        "<h3>Send a wireless data packet</h3>"
        "<form>"
          "<p>"
    "Data bytes <input type=text name=b size=50> (decimal)<br>"
    "Destination node <input type=text name=d size=3> "
        "(1..31, or 0 to broadcast)<br>"
          "</p>"
          "<input type=submit value=Send>"
        "</form>"), okHeader);
   
}

void loop(){
    word len = ether.packetReceive();
    word pos = ether.packetLoop(len);
    // check if valid tcp data is received
    if (pos) {
        bfill = ether.tcpOffset();
        char* data = (char *) Ethernet::buffer + pos;
#if SERIAL
        Serial.println(data);
#endif
        // receive buf hasn't been clobbered by reply yet
        if (strncmp("GET / ", data, 6) == 0)
            homePage(bfill);
        else if (strncmp("GET /c", data, 6) == 0)
            configPage(data, bfill);
        else if (strncmp("GET /s", data, 6) == 0)
            sendPage(data, bfill);
        else
            bfill.emit_p(PSTR(
                "HTTP/1.0 401 Unauthorized\r\n"
                "Content-Type: text/html\r\n"
                "\r\n"
                "<h1>401 Unauthorized</h1>"));  
        ether.httpServerReply(bfill.position()); // send web page data
    }
    
    doStromCount();

}
