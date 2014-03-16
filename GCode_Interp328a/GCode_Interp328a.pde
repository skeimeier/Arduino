// Arduino 328 G-code Interpreter 
// Arduino v1.0 by Mike Ellery - initial software (mellery@gmail.com)
// v1.1 by Zach Hoeken - cleaned up and did lots of tweaks (hoeken@gmail.com)
// v1.2 by Chris Meighan - cleanup / G2&G3 support (cmeighan@gmail.com)
// v1.3 by Zach Hoeken - added thermocouple support and multi-sample temp readings. (hoeken@gmail.com)
// Sanguino v1.4 by Adrian Bowyer - added the Sanguino; extensive mods... (a.bowyer@bath.ac.uk)
// v1.4.1 hacked to use the Arduino ATmega 328 and MCP23S17 I/O expanders -- by Mike Cook (mike@thebox.myzen.co.uk)
/*
* Hardware description
* ATmega 328 @ 16MHz with two MCP23S17 SPI port expander chips
* Expander chip at 0x40 called Limit. A side hand consol controls - B side movement limit switches
* Interrupt input connected to Arduino Pin 17 (PORT C3)
*  Expander chip at 0x42 called Motor. A side step / direction and enable for all three axies
* LCD connected to PORT D
*/
#include <ctype.h>
#include <HardwareSerial.h>
#include "WProgram.h"
#include "parameters.h"
#include "pins.h"
//#include <LiquidCrystalM.h>
#include <LiquidCrystalDirect.h>
#include <Spi.h>

// LiquidCrystalM lcd(LCD_RS, LCD_RW, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
 LiquidCrystalDircet lcd(LCD_RS, LCD_RW, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
SPI SPI();  // initilise SPI object


// Limit switches
  bool x_max_OK=true;
  bool x_min_OK=true;
  bool y_max_OK=true;
  bool y_min_OK=true;
  bool z_max_OK=true;
  bool z_min_OK=true;
  bool tool_height_OK=true;
// Our interrupt function
int tcount = 0;

void setup()
{
	pinMode(MOTOR_RESET_PIN, OUTPUT);
        digitalWrite(MOTOR_RESET_PIN, LOW); // start reset of stepper motor drivers
        pinMode(SPINDLE_PIN, OUTPUT); // set up spindle control pin
        analogWrite(SPINDLE_PIN, 0);  // turn off spindle
        pinMode(LIMIT_CHANGE_PIN, INPUT);   // for int from limit switch expander
        digitalWrite(LIMIT_CHANGE_PIN, HIGH); // enable internal pullups
        pinMode(HANDSET_CHANGE_PIN, INPUT);   // for int from handset switch expander
        digitalWrite(HANDSET_CHANGE_PIN, HIGH); // enable internal pullups
        Serial.begin(9600);
	Serial.println("starting");

        // initilise SPI expanders
        // Set up ICON register it defaults to address 0xA on reset:
        expanderW(MOTOR_WRITE, 0x0A, BANK | SEQOP | HAEN); // this sets both / all expanders to use address pins
        expanderW(LIMIT_WRITE, 0x0A, BANK | SEQOP | HAEN); // this sets both / all expanders to use address pins       
        expanderW(MOTOR_WRITE,IODIRA, 0x00);      // Data direction register A all outputs
        expanderW(MOTOR_WRITE,IODIRB, 0x00);      // Data direction register B all outputs
        expanderW(MOTOR_WRITE, OLATB, 0x00);      // disable motors, set dir to 0, set step to 0
        
        expanderW(LIMIT_WRITE,IODIRA, 0xff);      // Data direction register A all inputs
        expanderW(LIMIT_WRITE,IODIRB, 0xff);      // Data direction register B all inputs
        expanderW(LIMIT_WRITE,IPOLA, 0xff);       // Input polarity read an earth (press) as a one
        expanderW(LIMIT_WRITE,IPOLB, 0xff);       // Input polarity read an earth (press) as a one
        expanderW(LIMIT_WRITE,INTCONB, 0x00);     // Notify on change
        expanderW(LIMIT_WRITE,INTCONB, 0x00);     // Notify on change
        expanderW(LIMIT_WRITE,GPINTENA, 0xff);    // enable notifacation on pins
        expanderW(LIMIT_WRITE,GPINTENB, 0xff);    // enable notifacation on pins
	
	//other initialization.
	init_steppers();
	init_process_string();
        lcd.begin(2, 40);  // size of LCD
        lcd.clear();
        lcd.print("Mike's Miller - Initialised V0.2a");
        Serial.println("ok");
        digitalWrite(MOTOR_RESET_PIN, HIGH);  // stepping motor drives release reset
}

void loop()
{
#ifdef TEST_MACHINE  

// Run the parts of the machine as a test

  byte c = 0;
  if (Serial.available() > 0)
  {
        c = Serial.read();
        Serial.println(c);
    switch(c)
    {      
      case 'x':
        X_motor_test();
        break;
      case 'y':
        Y_motor_test();
        break;
      case 'z':
        Z_motor_test();
        break;
      
    default:
      Serial.println("Commands:\n");
      Serial.println(" x - X motor test");
      Serial.println(" y - Y motor test");
      Serial.println(" z - Z motor test");
    }
        Serial.print("Command: ");
  }
  else  // when nothing recieved
  {
 
  }

#else   // do the real thing

        get_and_do_command(); 
    
#endif        
}

byte expanderR(byte com,byte add) 
{
  byte value;
  digitalWrite(SS_PIN, LOW);
  Spi.transfer(com);  // address read
  Spi.transfer(add);   //  register address
  value = Spi.transfer(0x0);   //  dummy data for read
  digitalWrite(SS_PIN, HIGH);
  return value;
}

byte expanderW(byte com, byte add, byte dat) // expander read
{
  digitalWrite(SS_PIN, LOW);
  Spi.transfer(com);  // address write
  Spi.transfer(add);   //  register address
  Spi.transfer(dat);   //  register data
  digitalWrite(SS_PIN, HIGH);
}

