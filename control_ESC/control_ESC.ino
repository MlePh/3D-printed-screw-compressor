#include <Servo.h>
Servo servoESC;

void setup() {
  
  Serial.begin(9600);       // start serial port
  servoESC.attach(9);       // servo signal cable connected to D9

}

void loop() {

  int val;                             // variable for potentiometer reading

  val = analogRead(A0);                // read the potentiometer value
  val = map(val, 0, 1023, 1000, 2000); // map it between 1000 and 2000 microseconds

  servoESC.writeMicroseconds(val);     // send value to ESC
  Serial.println(val);                 // send value to seria 

}
