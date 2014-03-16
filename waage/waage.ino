
#include <Ports.h>
#include <RF12.h> // needed to avoid a linker error :(

Port waagepulse (4);   // puls ausgang is connected to DIO (pin 2) of port 4
Port duoled (3);       // duoLED (gemeinsame Kathode) an DIO(grÃ¼n) und AIO(rot) port 3

int count;
int state;
long lastmilli;
long dauer,calib,last,lastlast;

void setup() {
  Serial.begin(57600);

  Serial.println("\n[waage_demo]");
  rf12_config();
  // set up easy transmissions every 5 seconds
  rf12_easyInit(5);

  waagepulse.mode(INPUT);
  duoled.mode(OUTPUT);
  duoled.mode2(OUTPUT);
  duoled.digiWrite(1);
  delay(200);              // wait for a second
  duoled.digiWrite(0);
  delay(200);              // wait for a second

  duoled.digiWrite2(1);
  delay(200);              // wait for a second
  duoled.digiWrite2(0);

  // pir.mode2(INPUT);
  //waagepulse.digiWrite2(1); // pull-up
  state = 0;
  count = 0;

}


void loop() {
  rf12_easyPoll();

  if (waagepulse.digiRead() == 0) {
    lastmilli = micros();
    while(waagepulse.digiRead() != 1)
      lastlast =  micros();
    ;
    dauer = micros() - lastmilli;
    Serial.println(dauer,DEC);

    if(state == 0){
      duoled.digiWrite(0);
      duoled.digiWrite2(1);

      if( (dauer >30000) && (dauer < 50000) ){
        calib += dauer;
        count++;
      }
      if(count >= 5){ 
        calib = calib / count;
        count = 0;
        state++;
        Serial.print("\nCalib: ");
        Serial.println(calib,DEC);
      } 

    }
    if(state == 1){
      duoled.digiWrite(1);
      duoled.digiWrite2(1);
      if(dauer > (calib+2000)){
        //lastlast = last;
        if( (dauer <= (last+100))  && (dauer >= (last-100)) ) {
          count++;
        }
        else{
          count = 0;
        }
        // Serial.println(count,DEC);
        last = dauer;

      }
      if(count == 3){
        Serial.print("\nWaage ");
        Serial.println(dauer-calib,DEC);
        int value = (dauer-calib)/100;
        rf12_easySend(&value, sizeof value);
        duoled.digiWrite(1);
        duoled.digiWrite2(0);

        state++;
      }
    }
  }
  if(state == 2){
    if((micros() - lastlast) > 1000000 ){
      state = 0;
      count = 0;
      calib = 0;
      duoled.digiWrite(0);
      duoled.digiWrite2(0);
    }
  }
}


