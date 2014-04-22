String numExpand(byte num){
   String twochr = "xx";
   twochr.setCharAt(1,'0'+(num%10));
   twochr.setCharAt(0,'0'+(num/10));
   
   return twochr;
  
}

void setup(){
     Serial.begin(57600);

  String myString = numExpand(23);
 
 Serial.println(myString);
 myString = numExpand(2
 );
 Serial.println(myString);
 
}

void loop(){

delay(1000);

}
