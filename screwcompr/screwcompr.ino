/*
 3D printed screw compressor
 
 Controlling the ESC from the screw compressor to work as a ventilator by
 using two potentiometers - one for speed and one for the time interval.

 last modified 02.04.2020
 by Mlekusch Philipp
*/

//######################################################################################
//#################### change parameters if you need to ################################
//######################################################################################

// percentages of the sequences, multiplied by 100 to get a better resolution
// sum should be 10000 = 100%

const int percRampUp = 2000;      // Ramp Up time in percents of one cycle
const int percHighFlow = 1000;    // time span at high RPMs (breath in)
const int percRampDown = 3500;    // Ramp Down time in percent of one cycle
const int percIdle = 3500;        // time span at idle RPMs (breath out)

const int idleSpeed = 1050;       // idle speed of the motor in microseconds

const int timeMin = 2000;         // minimum cycle time in milliseconds
const int timeMax = 8000;         // maximum cycle time in milliseconds

const int refreshrate = 50;       // refreshrate of the loop

//######################################################################################

#include <Servo.h>
Servo servoESC;     // servo object to control the screw compressor ESC
int valESC;         // variable for the value send to the ESC 

int potSpeed;      // potentiometer to regulate the motor speed connected to Pin A0
int potTime;       // potentiometer to regulate the time interval connected to Pin A1

int timerMs;       // timer value in miliseconds
int timerPerc;     // timer value in percents, mutliplied by 100

void setup() {

  Serial.begin(9600);       // start serial port
  servoESC.attach(9);       // servo signal cable is connected to D9

  timerMs = 0;             // set timer to zero

  // ESC startup sequence
  servoESC.writeMicroseconds(1000);       // set to 1000 microseconds (lowest value)
  delay(5000);                            // wait 5 seconds to give ESC time to initialize
  servoESC.writeMicroseconds(idleSpeed);  // set to ESC idle before first cycle
  delay(5000);                            // idle for 5 seconds

  // set initial cycle time
  potTime = analogRead(A1);
  potTime = map(potTime, 0, 1023, timeMin, timeMax);  
    
}

void loop() {

  // read speed potentiometer values
  potSpeed = analogRead(A0);   // speed range between 1200 and 2000 microseconds
  potSpeed = map(potSpeed, 0, 1023, 1200, 2000);

  // sequence boundaries
  int percRampUpStart = 0;
  int percRampUpEnd = percRampUp - 1;
  int percHighFlowStart = percRampUp;
  int percHighFlowEnd = percRampUp + percHighFlow - 1;
  int percRampDownStart = percRampUp + percHighFlow;
  int percRampDownEnd = percRampUp + percHighFlow + percRampDown - 1;
  int percIdleStart = percRampUp + percHighFlow + percRampDown;
  int percIdleEnd = percRampUp + percHighFlow + percRampDown + percIdle - 1;

  // maping the time in miliseconds to percentage
  timerPerc = map(timerMs, 0, potTime, 0, percIdleEnd);

  // going through the sequences of one cycle

  // RAMP UP sequence
  if ((timerPerc > percRampUpStart) && (timerPerc < percRampUpEnd)) {
      float rampUpRad = ( 100 / float(percRampUpEnd - percRampUpStart) * timerPerc ) / 100 * 3.14; 
      float rampUpCos = cos(rampUpRad);
                         
      valESC = map(int(rampUpCos * -10000), -10000, 10000, idleSpeed, potSpeed);
  }
  
  // HIGH FLOW sequence
  else if ((timerPerc > percHighFlowStart) && (timerPerc < percHighFlowEnd)) {

      valESC = potSpeed;
  } 
  
  // RAMP DOWN requence
  else if ((timerPerc > percRampDownStart) && (timerPerc < percRampDownEnd)) {
      float rampDownRad = ( 100 / float(percRampDownEnd - percRampDownStart) * (timerPerc - percRampDownStart) ) / 100 * 3.14; 
      float rampDownCos = cos(rampDownRad);
            
      valESC = map(int(rampDownCos * -10000), -10000, 10000, potSpeed, idleSpeed);
  }
  
  // IDLE sequence
  else if ((timerPerc > percIdleStart) && (timerPerc < percIdleEnd)) {
                    
      valESC = idleSpeed;
  }
  
  Serial.println(valESC);

  servoESC.writeMicroseconds(valESC);

  timerMs += refreshrate;
  delay(refreshrate);        // delay in between reads for stability
  
  if (timerMs > potTime) {
    
    //set cycle time
    potTime = analogRead(A1);    // cycle time - range between 3000 and 10000 miliseconds
    potTime = map(potTime, 0, 1023, timeMin, timeMax);
    
    timerMs = 0;
    
  }
  
}
