/// @dir Ports/lcd_demo
/// my Demo sketch for an LCD connected to I2C port via MCP23016 I/O expander.
// 2014-04-28 SK

#include <PortsLCD.h>

PortI2C myI2C (1);
LiquidCrystalMyI2C lcd (myI2C);

#define screen_width 20
#define screen_height 4

void setup() {
  // set up the LCD's number of rows and columns: 
  delay(500);
  lcd.begin(screen_width, screen_height);
  if(screen_height == 4)
      lcd.set4Lines();
  // Print a message to the LCD.
  lcd.print("Hello, world!");
  delay(5000);
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.clear();
  for (int num=0;num<10;num++) {
    for (int y=0;y<screen_height;y++) {
      lcd.setCursor(0,y);
      lcd.print(y);
      lcd.print(')');
      for (int x=2;x<screen_width;x++) {
        lcd.setCursor(x,y);
        lcd.print(num);
      }
    }
    delay(200);
  }
  lcd.clear();
  for (int ascii=0;ascii<=screen_width*screen_height;ascii++) {
    //lcd.setCursor(ascii % screen_width,ascii / screen_width);
    lcd.print(char(48+ascii)); // type cast the value as a char to force print to display the char, not the value
  }
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print("Up time:");
  lcd.print(millis()/1000);
  lcd.print('s');
  // turn the backlight off, briefly
  delay(500);
  for (int i=0;i<4;i++) {
    lcd.noBacklight();
    delay(50);
    lcd.backlight();
    delay(50);
  }
  delay(450);
}
