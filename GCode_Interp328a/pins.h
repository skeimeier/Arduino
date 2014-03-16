#ifndef PINS_H
#define PINS_H

/****************************************************************************************
* Arduino pin assignment
*
****************************************************************************************/
// Step, direction & enable are on SPI expander  bit mask
#define X_STEP_BIT (byte)0x1
#define X_DIR_BIT (byte)0x8

#define Y_STEP_BIT (byte)0x2
#define Y_DIR_BIT (byte)0x10

#define Z_STEP_BIT (byte)0x4
#define Z_DIR_BIT (byte)0x20

#define XY_ENABLE_BIT (byte)0x40
#define Z_ENABLE_BIT (byte)0x80

byte stepperByte;

#define SPINDLE_PIN (byte) 9

// Endstop switches on SPI expander bit mask
#define X_MAX_BIT (byte) 0x1
#define X_MIN_BIT (byte) 0x2
#define Y_MAX_BIT (byte) 0x4
#define Y_MIN_BIT (byte) 0x8
#define Z_MAX_BIT (byte) 0x10
#define Z_MIN_BIT (byte) 0x20
#define TOOL_HEIGHT_BIT (byte) 0x40

// LiquidCrystal display with:
// rs on pin 3
// rw on pin 2
// enable on pin 8
// d4, d5, d6, d7 on pins 4, 5, 6, 7
// allows direct port access
#define LCD_RS (byte)3
#define LCD_RW (byte)2
#define LCD_EN (byte)8
#define LCD_D4 (byte)4
#define LCD_D5 (byte)5
#define LCD_D6 (byte)6
#define LCD_D7 (byte)7
byte lcdByte;

// MCP23S17 addresses in IOCON.BANK = 1
#define IODIRA (byte) 0x00
#define IODIRB (byte) 0x10
#define IPOLA (byte) 0x01
#define IPOLB (byte) 0x11
#define GPINTENA (byte) 0x02
#define GPINTENB (byte) 0x12
#define DEFVALA (byte) 0x03
#define DEFVALB (byte) 0x13
#define INTCONA (byte) 0x04
#define INTCONB (byte) 0x14
#define IOCON (byte) 0x05
#define GPPUBA (byte) 0x06
#define GPPUBB (byte) 0x16
#define INTFA (byte) 0x07
#define INTFB (byte) 0x17
#define INTCAPA (byte) 0x08
#define INTCAPB (byte) 0x18
#define GPIOA (byte) 0x09
#define GPIOB (byte) 0x19
#define OLATA (byte) 0x0A
#define OLATB (byte) 0x1A
// Bits in the IOCON register
#define BANK (byte) 0x80
#define MIRROR (byte) 0x40
#define SEQOP (byte) 0x20
#define DISSLW (byte) 0x10
#define HAEN (byte) 0x08
#define ODR (byte) 0x04
#define INTPOL (byte) 0x02

#define LIMIT_READ (byte) 0x41
#define LIMIT_WRITE (byte) 0x40
#define MOTOR_READ (byte) 0x49
#define MOTOR_WRITE (byte) 0x48

#define LIMIT_CHANGE_PIN (byte)16
#define HANDSET_CHANGE_PIN (byte)17

#define MOTOR_RESET_PIN (byte)9

#endif
