//led.ino
#define LED D0  // connect D0 pin to LED 
void setup(){
  pinMode(LED,OUTPUT); // setup output
}

void loop(){
  digitalWrite(LED,HIGH); // Pin D0 is HIGH
  delay(250);
  digitalWrite(LED,LOW); // Pin D0 is LOW
  delay(1000); //delay for one second.
}
