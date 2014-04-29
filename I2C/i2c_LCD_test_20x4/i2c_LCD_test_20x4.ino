

#include <Ports.h>

PortI2C myI2C (1);
DeviceI2C lcd (myI2C,0x20);

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00



enum { 
  MCP_GP0, MCP_GP1, MCP_OLAT0, MCP_OLAT1, MCP_IPOL0,
  MCP_IPOL1, MCP_IODIR0, MCP_IODIR1, MCP_INTCAP0,
  MCP_INTCAP1,MCP_IOCON0, MCP_IOCON1
};
// bits 0..3 and D4..D7, the rest is connected as follows

#define MCP_BACKLIGHT   0x10
#define MCP_ENABLE      0x80
#define MCP_RW          0x40
#define MCP_REGSEL      0x20

char controll = MCP_BACKLIGHT;
char displayfunction = LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKON;
char numlines = 4;
char numcols = 20;

void lcd_write(char data){
  //controll = MCP_REGSEL;
  lcd.send();
  lcd.write(MCP_GP0);
  //delayMicroseconds(20);
  lcd.write(controll);
  delayMicroseconds(20);

  lcd.write(data);
  delayMicroseconds(20);
  controll |= MCP_ENABLE;
  lcd.write(controll);
  delayMicroseconds(20);
  lcd.write(data);
  delayMicroseconds(20);
  //delayMicroseconds(20);
  controll &= ~MCP_ENABLE;
  lcd.write(controll);
  delayMicroseconds(20);
  lcd.write(data);
  delayMicroseconds(20);
  lcd.stop();   

}

void lcd_command(char data) {
  controll &= ~MCP_REGSEL;
  lcd_write(data);   

}
void lcd_data(char data) {
  controll |= MCP_REGSEL;
  lcd_write(data);   
}

void lcd_lightOn(){
  controll |=  MCP_BACKLIGHT;
  lcd_command(0);
}

void lcd_lightOff(){
  controll &= ~MCP_BACKLIGHT;
  lcd_command(0);    
}

void lcd_displayOn(){
  displayfunction |= LCD_DISPLAYON;
  lcd_command(displayfunction );
}

void lcd_displayOff(){
  displayfunction &= ~LCD_DISPLAYON;
  lcd_command(displayfunction );
}

void lcd_cursorOn(){
  displayfunction |= LCD_CURSORON;
  lcd_command(displayfunction );
}

void lcd_cursorOff(){
  displayfunction &= ~LCD_CURSORON;
  lcd_command(displayfunction );
}

void lcd_blinkOn(){
  displayfunction |= LCD_BLINKON;
  lcd_command(displayfunction );
}

void lcd_blinkOff(){
  displayfunction &= ~LCD_BLINKON;
  lcd_command(displayfunction );
}

void lcd_setCursor(byte row, byte col)
{
  int row_offsets[] = { 
    0x00, 0x20, 0x40, 0x60   };
  if ( row > numlines ) {
    row = numlines-1;    // we count rows starting w/0
  }
  lcd_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

char lcd_custom_0[] = { 
  0x15, 0x1a, 0x15, 0x1a, 0x15, 0x10, 0x10, 0x10}; // checked flag 

int lcd_defineCharacter(int ascii, char *data) {
  int baseAddress = (ascii * 8) + 64;  
  // baseAddress = 64 | (ascii << 3);
  lcd_command(baseAddress);
  for (int i = 0; i < 8; i++)
    lcd_data(data[i]);
  lcd_command(128);
  return ascii;
} 


// BIG FONT

PROGMEM prog_char pbf_lcd_ch0[]={
  64,64,64,64,64,64,64,64,0};// 0, Empty
PROGMEM prog_char pbf_lcd_ch1[]={
  64,64,64,64,64,31,31,31,0};//1, Lower 3 lines 220
PROGMEM prog_char pbf_lcd_ch2[]={
  64,64,64,31,31,64,64,64,0};//2, middle 2 lines
PROGMEM prog_char pbf_lcd_ch3[]={
  64,64,64,31,31,31,31,31,0};//3, lower 5 lines
PROGMEM prog_char pbf_lcd_ch4[]={
  31,31,31,64,64,64,64,64,0};//4, upper 3 lines 223
PROGMEM prog_char pbf_lcd_ch5[]={
  31,31,31,64,64,31,31,31,0};//5, upper 3 lines + lower 3 lines
PROGMEM prog_char pbf_lcd_ch6[]={
  31,31,31,31,31,64,64,0};//6, upper 5 lines
PROGMEM prog_char pbf_lcd_ch7[]={
  31,31,31,31,31,31,31,31,0};//7, all 7 lines 219
PROGMEM const char *pbf_ch_item[] = {
  pbf_lcd_ch0, pbf_lcd_ch1, pbf_lcd_ch2, pbf_lcd_ch3, pbf_lcd_ch4, pbf_lcd_ch5, pbf_lcd_ch6, pbf_lcd_ch7};

char font_35_09[]={
  1,2,3,7,1,6, 1,3,0,0,7,0, 2,2,3,7,5,5, 2,2,3,5,5,7, 3,0,3,4,4,7, 3,2,2,5,5,7, 3,2,2,7,5,7, 2,2,3,0,0,7, 3,2,3,7,5,7, 3,2,3,5,5,7};

void lcd_init_big_font()
{
  char ch_buffer[10]; // This buffer is required for custom characters on the LCD.
  for (int i=0;i<8;i++)
  {
    strcpy_P((char*)ch_buffer,(char*)pgm_read_word(&(pbf_ch_item[i])));
    lcd_defineCharacter(i,ch_buffer);
  }
}

void lcd_render_big_char(char ch, byte loc_x, byte loc_y)
{
  lcd_setCursor(loc_x,loc_y);
  if ((ch>='0')&&(ch<='9'))
  {
    lcd_setCursor(loc_x,loc_y);
    for (byte i=0; i<3; i++) lcd_data(font_35_09[(ch-'0')*6+i]);  //TOP
    lcd_setCursor(loc_x+1,loc_y);
    for (byte i=0; i<3; i++) lcd_data(font_35_09[(ch-'0')*6+i+3]);  //bottom   
  }

}

void setup() {
  Serial.begin(57600);

  Serial.println("\n[expander]");
  bool ok = lcd.isPresent();
  Serial.println(ok?"da":"weg");
  if(ok){
    Serial.println("Init");
    lcd.send();
    lcd.write(MCP_IODIR0);
    delayMicroseconds(20);
    lcd.write(0x00);
    delayMicroseconds(20);
    lcd.write(0x00);
    delayMicroseconds(20);
    lcd.stop();
    delay(1000);
    lcd.send();
    lcd.write(MCP_GP0);
    delayMicroseconds(20);
    lcd.write(0xFF);
    delayMicroseconds(20);
    lcd.write(0xFF);
    delayMicroseconds(20);
    lcd.stop();

    //while(Serial.available() == 0){};Serial.read();

    lcd_command(0x30);      
    Serial.println("0x20");
    delayMicroseconds(4500);  // wait more than 4.1ms
    lcd_command(0x30);      
    delayMicroseconds(150);
    lcd_command(0x30);      
    delayMicroseconds(150);
    lcd_command(0x30);      
    delayMicroseconds(150);
    //lcd_command(0x24);      
    // lcd_command(0x09);      
    // lcd_command(0x20);   

    lcd_command(0x3C);      
    Serial.println("0x3C");

    //while(Serial.available() == 0){};Serial.read();

    delayMicroseconds(1);
    lcd_command(0x09);      
    Serial.println("0x09");
    // while(Serial.available() == 0){};Serial.read();
    delayMicroseconds(1);
    lcd_command(0x38);      
    Serial.println("0x38");
    // while(Serial.available() == 0){};Serial.read();
    delayMicroseconds(1);
    lcd_command(0x0E);      
    Serial.println("0x0E");
    //   while(Serial.available() == 0){};Serial.read();
    delayMicroseconds(1);


    // lcd_command(LCD_FUNCTIONSET | LCD_2LINE); 
    //lcd_command(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKON );
    lcd_command(1 );  // clr
    // while(Serial.available() == 0){};Serial.read();
    delayMicroseconds(150);

    lcd_command(0x02); //Home
    delayMicroseconds(150);
    // while(Serial.available() == 0){};Serial.read();

    lcd_defineCharacter(0, lcd_custom_0);
    lcd_init_big_font();   
    lcd_setCursor(0,0);    
    lcd_data('1');
    lcd_setCursor(1,0);    
    lcd_data('2');
   /* lcd_setCursor(2,0);    
    lcd_data('3');
    lcd_setCursor(3,0);    
    lcd_data('4');
  */
    delay(1000);
    lcd_setCursor(0,9);    
    lcd_data('1');
    lcd_setCursor(1,9);    
    lcd_data('2');
   /* lcd_setCursor(2,9);    
    lcd_data('3');
    lcd_setCursor(3,9);    
    lcd_data('4');
*/
    delay(1000);
    lcd_data(0);
    lcd_data(1);
    lcd_data(2);
    lcd_data(3);
    lcd_data(4);
    lcd_data(5);
    lcd_data(6);
    lcd_data(7);
    delay(1000);

    lcd_render_big_char('0',1,0);
    lcd_render_big_char('1',1,4);
    lcd_render_big_char('2',1,8);
    lcd_render_big_char('3',1,12);
    delay(3000);

  }

}

void loop() {	
  static int count = 0;
  delay(1000);
  Serial.println("Hallo");
  lcd_command(1 );  // clr
  delayMicroseconds(150);
  lcd_command(2 );  // home
  delayMicroseconds(50);

  lcd_data('H');
  lcd_data('a');
  lcd_data('l');
  lcd_data('l');
  lcd_data('o');
  lcd_data(':');
  lcd_data(' ');
  String out =  String(millis(), DEC);             // using a long and a base
  Serial.println(out);
  for(int i=0;i<out.length();i++){
    lcd_data(out.charAt(i) );
    Serial.println(i);

  }
  /*
  (count++ % 20) ? lcd_lightOn() : lcd_lightOff();
  (count % 10) ? lcd_displayOn() : lcd_displayOff();
  (count % 2) ? lcd_blinkOn() : lcd_blinkOff();
  (count % 5) ? lcd_cursorOn() : lcd_cursorOff();
  lcd_setCursor(3,0); 
  for(int i=0;i<8;i++){
    lcd_data(i);
  };
/*
  lcd_render_big_char('0',1,0);
  lcd_render_big_char('1',1,4);
  lcd_render_big_char('2',1,8);
  lcd_render_big_char('3',1,12);
*/
  lcd_setCursor(1,0); 
  lcd_data('2'); 
  lcd_setCursor(2,0); 
  lcd_data('3');  
  lcd_setCursor(3,0); 
  lcd_data('4');  
  lcd_setCursor(0,14);   
/*
  out =  String(count % 100, DEC);             // using a long and a base
  //Serial.println(out);
  for(int i=0;i<out.length();i++){
    lcd_data(out.charAt(i) );
  }
  for(int i=0;i<out.length();i++){
      lcd_render_big_char(out.charAt(i),1,0+i*4);
  }
 lcd_setCursor(3,19);   
 */
 delay(5000);
}



