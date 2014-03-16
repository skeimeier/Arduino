

#include <Ports.h>

PortI2C myI2C (1);
DeviceI2C DL1414 (myI2C,0x20);



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
 
   DL1414_write(data); 
} 
  



void DL1414_charOut(char c){
  DL1414_set_adr();
  DL1414_set_displ();
   data = c;
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
    pos = 0;
    
}

void loop() {	
  static int count = 0;
  delay(1000);
  
  Serial.println("HALLO");
  DL1414_charOut('H');
  DL1414_charOut('A');
  DL1414_charOut('L');
  DL1414_charOut('L');
  DL1414_charOut('O');
  DL1414_charOut(':');
  DL1414_charOut(' ');
  if(pos == (numcols-1) ){
   DL1414_clear();
   delay(300);
   pos = 0
   ; 
  }
 
}



