#include <SPI.h>

  inline static void initSS()    { DDRB  |=  _BV(2); };
  inline static void setSS()     { PORTB &= ~_BV(2); };
  inline static void resetSS()   { PORTB |=  _BV(2); };

uint8_t chipID;
uint8_t result;


void setup() {
  Serial.begin(9600);
    Serial.println("WIZ_chip_TEST");

  // start the SPI library:
    delay(300);
    initSS();
    SPI.begin();

    chipID = SPIread(0x0039, 0);
    Serial.print("chipID=");
    Serial.println(chipID);

    result = SPIread(0x00, 0);
    Serial.print("mode Register chipID=");
    Serial.println(result,BIN);
    
    Serial.print("GW_IP = ");  
    for(uint16_t i=0;i<4;i++){
      result = SPIread(1+i, 0);
      Serial.print(result);
      Serial.print(".");    
    }
    Serial.println();

//set myIP
    //for(uint16_t i=0;i<4;i++){
      result = SPIwrite(0x0F+0, 0x04, 192);
      result = SPIwrite(0x0F+1, 0x04, 168);
      result = SPIwrite(0x0F+2, 0x04, 115);
      result = SPIwrite(0x0F+3, 0x04, 177);
      
    //  Serial.print(result);
    //  Serial.print(".");    
    //}

//read myIP
    Serial.print("MY_IP = ");  
    for(uint16_t i=0;i<4;i++){
      result = SPIread(0x0F+i, 0);
      Serial.print(result);
      Serial.print(".");    
    }
    Serial.println();


}

uint8_t SPIwrite(uint16_t _addr, uint8_t _cb, uint8_t _data)
{
    setSS();  
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    SPI.transfer(_cb);
    SPI.transfer(_data);
    resetSS();
    return 1;
}

uint8_t SPIread(uint16_t _addr, uint8_t _cb)
{
    setSS();
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    SPI.transfer(_cb);
    uint8_t _data = SPI.transfer(0);
    resetSS();

    return _data;
}

void loop(){

}
