
#define LED_1 3   // the PWM pin which drives the red LED
#define LED_2 5   // the PWM pin which drives the green LED
#define LED_3 6   // the PWM pin which drives the blue LED


word counter;

void setup() {}

void loop () {
  
  byte level = ++ counter;
  analogWrite(LED_1, bitRead(counter,8) ? level : 0);
  analogWrite(LED_2, bitRead(counter,9) ? level : 0);
  analogWrite(LED_3, bitRead(counter,10) ? level : 0);
  delay(5);
}

