

#include <Ports.h>

#define MDATA 5
#define MCLK 15

PortI2C myI2C (1);
DeviceI2C DL1414 (myI2C,0x20);


#include <stdarg.h>
 void DL1414_print(char *fmt, ... ){
         char tmp[17]; // resulting string limited to 16 chars
         va_list args;
         va_start (args, fmt );
         vsnprintf(tmp, 17, fmt, args);
         va_end (args);
         //Serial.print(tmp);
         int i=0;
         while(tmp[i]>0){
           DL1414_charOut(tmp[i++]);
         }
 }



enum { 
  MCP_GP0, MCP_GP1, MCP_OLAT0, MCP_OLAT1, MCP_IPOL0,
  MCP_IPOL1, MCP_IODIR0, MCP_IODIR1, MCP_INTCAP0,
  MCP_INTCAP1,MCP_IOCON0, MCP_IOCON1
};

char data = 0;
int controll = 0xff;
int numcols = 16;
int pos = 0;

void DL1414_write(char data){
  DL1414.send();
  DL1414.write(MCP_GP1);
  //delayMicroseconds(20);
  DL1414.write(data);
  //delayMicroseconds(20);
  DL1414.write(controll);
  //delayMicroseconds(20);
  DL1414.stop();

}

void DL1414_set_adr(){
  byte adress =  3 -  (pos & 0x03);
  //Serial.print("pos: ");
  //Serial.print(pos);
   //Serial.print("  adr: ");
  //Serial.print(adress,HEX);

 controll = 0b11111100 | adress;
   //Serial.print(" controll_1: ");
 
  //Serial.print(controll,BIN);
 
   DL1414_write(data); 
}


void DL1414_set_displ(){
  char displNo = pos >> 2;
  char thisbit = _BV(displNo);
 // Serial.print("wrbits: ");
  //Serial.println(thisbit,BIN);
  controll &=  ~(thisbit<<2)  ;
 DL1414_write(data); 
    //Serial.print(" controll_2: ");
 
 // Serial.print(controll,BIN);
 
 //  DL1414_write(data); 
} 
  



void DL1414_charOut(char c){
   data = c;
  DL1414_set_adr();
  DL1414_set_displ();
//   data = c;
  //Serial.print(" Chr: " );
   //Serial.print( c );
   DL1414_set_adr();
   // Serial.println();
 
  if(pos < (numcols-1) ) pos++;

 
} 

void DL1414_clear(){
   // Serial.println("Clear    ");
   pos = 0;
 for(int i = 0; i< numcols;i++ ){
      DL1414_charOut(' ');
  };
  pos = 0;
}

  int xcount;
  int ycount;

/*
 * according to some code I saw, these functions will
 * correctly set the mouse clock and data pins for
 * various conditions.
 */
void gohi(int pin)
{
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH);
}

void golo(int pin)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void mouse_write(char data)
{
  char i;
  char parity = 1;

  //  Serial.print("Sending ");
  //  Serial.print(data, HEX);
  //  Serial.print(" to mouse\n");
  //  Serial.print("RTS");
  /* put pins in output mode */
  gohi(MDATA);
  gohi(MCLK);
  delayMicroseconds(300);
  golo(MCLK);
  delayMicroseconds(300);
  golo(MDATA);
  delayMicroseconds(10);
  /* start bit */
  gohi(MCLK);
  /* wait for mouse to take control of clock); */
  while (digitalRead(MCLK) == HIGH)
    ;
  /* clock is low, and we are clear to send data */
  for (i=0; i < 8; i++) {
    if (data & 0x01) {
      gohi(MDATA);
    } 
    else {
      golo(MDATA);
    }
    /* wait for clock cycle */
    while (digitalRead(MCLK) == LOW)
      ;
    while (digitalRead(MCLK) == HIGH)
      ;
    parity = parity ^ (data & 0x01);
    data = data >> 1;
  }  
  /* parity */
  if (parity) {
    gohi(MDATA);
  } 
  else {
    golo(MDATA);
  }
  while (digitalRead(MCLK) == LOW)
    ;
  while (digitalRead(MCLK) == HIGH)
    ;
  /* stop bit */
  gohi(MDATA);
  delayMicroseconds(50);
  while (digitalRead(MCLK) == HIGH)
    ;
  /* wait for mouse to switch modes */
  while ((digitalRead(MCLK) == LOW) || (digitalRead(MDATA) == LOW))
    ;
  /* put a hold on the incoming data. */
  golo(MCLK);
  //  Serial.print("done.\n");
}

/*
 * Get a byte of data from the mouse
 */
char mouse_read(void)
{
  char data = 0x00;
  int i;
  char bit = 0x01;

  //  Serial.print("reading byte from mouse\n");
  /* start the clock */
  gohi(MCLK);
  gohi(MDATA);
  delayMicroseconds(50);
  while (digitalRead(MCLK) == HIGH)
    ;
  delayMicroseconds(5);  /* not sure why */
  while (digitalRead(MCLK) == LOW) /* eat start bit */
    ;
  for (i=0; i < 8; i++) {
    while (digitalRead(MCLK) == HIGH)
      ;
    if (digitalRead(MDATA) == HIGH) {
      data = data | bit;
    }
    while (digitalRead(MCLK) == LOW)
      ;
    bit = bit << 1;
  }
  /* eat parity bit, which we ignore */
  while (digitalRead(MCLK) == HIGH)
    ;
  while (digitalRead(MCLK) == LOW)
    ;
  /* eat stop bit */
  while (digitalRead(MCLK) == HIGH)
    ;
  while (digitalRead(MCLK) == LOW)
    ;

  /* put a hold on the incoming data. */
  golo(MCLK);
    //Serial.print("Recvd data ");
    //Serial.print(data, HEX);
  //  Serial.print(" from mouse\n");
  return data;
}

void mouse_init()
{
  gohi(MCLK);
  gohi(MDATA);
  //  Serial.print("Sending reset to mouse\n");
  mouse_write(0xff);
  mouse_read();  /* ack byte */
  //  Serial.print("Read ack byte1\n");
  mouse_read();  /* blank */
  mouse_read();  /* blank */
  //  Serial.print("Sending remote mode code\n");
  mouse_write(0xf0);  /* remote mode */
  mouse_read();  /* ack */
  //  Serial.print("Read ack byte2\n");
  delayMicroseconds(100);
}




void setup() {
  Serial.begin(57600);

  Serial.println("\n[expander]");
  bool ok = DL1414.isPresent();
  Serial.println(ok?"da":"weg");
  if(ok){
    Serial.println("Init");
    DL1414.send();
    DL1414.write(MCP_IODIR0);
    delayMicroseconds(20);
    DL1414.write(0x00);
    delayMicroseconds(20);
    DL1414.write(0x00);
    delayMicroseconds(20);
    DL1414.stop();
    delay(1000);
   }
   DL1414_clear();


  mouse_init();
   xcount=0;
   ycount=0;
   
}

void loop() {	
  static int count = 0;
  static char mstat,mstatold;
  char mx;
  char my;
  boolean reset, pitchplus;
  static int pitch=1;
  delay(10);
  
  //Serial.println("HALLO");
      pos = 0;

  DL1414_print("PITCH %1d CNT%5d",pitch,ycount/2/pitch);

  /* get a reading from the mouse */
  mouse_write(0xeb);  /* give me data! */
  mouse_read();      /* ignore ack */
  mstat = mouse_read();
  mx = mouse_read();
  my = mouse_read();
  if(mstat != mstatold){    
      reset = mstat & 0b10;
      mstatold = mstat;
      if(reset) ycount = 0;
      pitchplus = mstat & 0b1;
      if(pitchplus) pitch++;
      pitch = pitch>8 ? 1 : pitch;
    Serial.print(pitch, DEC);
     
  }
    
  xcount+=mx;
  ycount+=my;

  /* send the data back up */
  Serial.print(mstat, BIN);
  Serial.print("\tX=");
  Serial.print(mx, DEC);
  Serial.print("\tY=");
  Serial.print(my, DEC);
  Serial.print("\tX=");
  Serial.print(xcount, DEC);
  Serial.print("\tcnt=");
  Serial.print(ycount/2/pitch, DEC);
  Serial.println();
  delay(20);  /* twiddle */

}



