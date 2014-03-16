



// 2-dimensional array of data pin numbers:
const int data[7] = {
  2,3,4,5,6,7,8 };

// 2-dimensional array of column adress + disp pin numbers:
const int displ[4] = {
  16,17,18,19 };

const int adr[2] = {
  14,15 };

int pos = 0;
char x = 0x20;

void setup() {
  Serial.begin(9600);
  // initialize the I/O pins as outputs:

  // iterate over the pins:
  for (int thisPin = 0; thisPin < 7; thisPin++) {
    pinMode(data[thisPin], OUTPUT); 
    digitalWrite(data[thisPin], HIGH);    
  }
  for (int thisPin = 0; thisPin < 4; thisPin++) {
    pinMode(displ[thisPin], OUTPUT); 
    digitalWrite(displ[thisPin], HIGH);    
  }
  for (int thisPin = 0; thisPin < 2; thisPin++) {
    pinMode(adr[thisPin], OUTPUT); 
    digitalWrite(data[thisPin], LOW);    
  }
}

void set_adr(){
  int adress = pos & 0x03;
  //Serial.print("adr: ");
  //Serial.println(adress);

  digitalWrite(adr[0], (adress & 0x01) ? LOW : HIGH);    
  digitalWrite(adr[1], (adress & 0x02) ? LOW : HIGH);    

}

void set_displ(){
  int No = pos >> 2;
  // Serial.print("DisplayNo: ");
  // Serial.println(No );
  // erst alle deselctieren
  for (int thisPin = 0; thisPin < 4; thisPin++) {
    digitalWrite(displ[thisPin], HIGH);    
  }

  digitalWrite(displ[No],LOW);    
}

void charOut(char c){
  set_adr();
  set_displ();
  // Serial.print("Bits: ");

  for (int thisPin = 0; thisPin < 7; thisPin++) {
    digitalWrite(data[thisPin], c & _BV(thisPin) ? HIGH : LOW);    
    //Serial.print(c & _BV(thisPin) ? "1": "0");
  }
  //Serial.println();
  int No = pos >> 2;
  digitalWrite(displ[No],HIGH);    
}  

void loop(){
  //delay(1000);
  charOut(x++);
  pos++;
  charOut(x++);
  pos++;
  charOut(x++);
  pos++;
  charOut(x++);
  pos++;
  //  delay(10000);
  if(x>=0x60) x= 0x20;
  if(pos>=16){
    pos = 0; 
    delay(250
    );
  }

}


