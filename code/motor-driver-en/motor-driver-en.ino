// Master code for Arduino Uno for the Saddle Vibrator
// https://www.thingiverse.com/thing:3554455
// Version 1.0 (14.4.19)

#include <Wire.h>

byte inData[10];                                 // incomming data array for data from controller (make larger than you need)

const int  vibrationmotor = 3;                   // pin designation for vibrationmotor / vibration motor
const int  rotationmotor = 5;                    // pin designation for rotationmotor
const int  LED = 13;                             // LED showing debugging mode
const int delayTime = 10;                        // Ramp up and ramp down delay time is set to 10 by default

bool  vibrationMaxButton = 0;                    // internal variables for vibration max button on controller - vibration start button?
bool  rampModeButton = 0;
bool  rampModeButtonflag = 0;                    // check flag to see if button is being held down, debounce
bool  rampmode = 0;                              // flag for ramping mode on vibrationmotor
bool  pulseMode = 0;                             // flag for pulse mode on vibration motor
bool  flag = 0;                                  // dead man switch for connection, stop motors if no data
bool  debugflag = 0;                             // set 1 for debugflag mode to print serial updates

void setup()
{
  delay(500);                                    // allow controller to start first
  Wire.begin();                                  // join i2c bus (address optional for master)
  Serial.begin(9600);                            // set serial baud to 9600
}

void loop() {

  flag = 0;                                      // set connection flag to off to show data to stop motors if no data arrives
  Wire.requestFrom(8, 5);                        // request 5 bytes from slave device #8
  while (Wire.available()) {
    for (int i = 0; i <= 4; i++) {
      inData[i] = Wire.read(); - '0';            // read 1 byte from the wire buffer in to "inData[i]"  -'0' is to convert back to int from char  
    }
    vibrationMaxButton = inData[2];              // check to see if any buttons have been presed
    rampModeButton = inData[3];
    
    if (inData[4] == 1){
      debugflag = 1;                             // enter debug mode
      digitalWrite(LED, HIGH);                   // LED showing debugging mode, HIGH);
    }
    else
    {
      debugflag = 0;                             // exit debug mode
      digitalWrite(LED, LOW);                    // LED showing debugging mode, HIGH);
    }
    
    flag = 1;                                    // set connection flag to on to show data has arrived.
  }

  if (flag == 0) {                               // deadman (no connection) switch to stop motors
    for (int i = inData[0]; i == 0; i--) {       // decrease vibrationmotor and 1 speeds until stopped
      analogWrite(vibrationmotor, 0);
      delay(10);
    }
    for (int i = inData[1]; i == 0; i--) {
      analogWrite(rotationmotor, 0);
      delay(10);
    }
  }


  if (flag == 1) {                                // only continue if controller is connected (deadman switch check)

    // ***************** BUTTON 0 ROUTINES *****************

    if (vibrationMaxButton == 1) {                           // process button routine if vibrationMaxButton has been pressed
      vibrationMaxButtonpress();
    }

    // ***************** BUTTON 1 ROUTINES *****************

    if (rampModeButton == 1) {                           
      rampModeButtonflag = 1;                            // set button flag to make sure it does not continuously run the routine (debounce)
    }

    if ((rampModeButton == 0) && (rampModeButtonflag == 1)) {   // if button has been released reset vibrationbutton flag and process routine
      rampModeButtonflag = 0;
      if (rampmode == 0) {
        rampModeButtonpress();
      }
      else if (rampmode == 1) {
        rampmode = 0;
      }

    }

    // ****************** MOTOR  ROUTINES ******************

    if ((vibrationMaxButton == 0) && (rampModeButton == 0)) {       // no buttons have been pressed - set motor speed
      if (rampmode == 1) {
        inData[0] = 255;
      }
      analogWrite(vibrationmotor, inData[0]);           // PWM to output vibrationmotor port
      delay(10);
      analogWrite(rotationmotor, inData[1]);             // PWM to output rotationmotor port
    }
  }
  if (debugflag == 1) {
    showSerial();
    delay(1000);
  }
  else if (debugflag == 0) {
    delay(100);
  }
}

void vibrationMaxButtonpress() {                  // vibrationMaxButton has been pressed
  inData[0] = 255;                                // set vibrationmotor speed to 100%
  analogWrite(vibrationmotor, inData[0]);         // PWM to output vibrationmotor port
}

void rampModeButtonpress(delayTime) {                      // rampModeButton button has been pressed
  rampmode = 1;
  delayTime = delayTime;
  vibrationRampUp(delayTime);

  /*for (int i = inData[0]; i <= 255; i++) {        // slowly ramp motor speed to 100% from current potentiometer setting
    Serial.print(i);
    Serial.println(".");
    analogWrite(vibrationmotor, i);
    delay(delayTime);
  }*/
  Serial.println();
}



//TODO: ramp pulse vs ON OFF ON OFF pulse? below is a ramped pulse, still need to figure out how to implement an abrupt pulse by pushing the pulse button X times to cycle through settings ?

void vibrationRampUp(delayTime) {
  //delayTime = inData[0];  //delayTime should be inData[0] if we want the ramp to be controlled by the potentiometer when called outside of rampModeButtonPress
for (int i = inData[0]; i <= 255; i++) {        
    Serial.print(i);
    Serial.println(".");
    analogWrite(vibrationmotor, i);
    delay(delayTime); //delay based on the current potentiometer setting for the vibration motor controller
}

void vibrationRampDown(delayTime) {

  for (int i = inData[0]; i >= 255; i--) {       
    Serial.print(i);
    Serial.println(".");
    analogWrite(vibrationmotor, i);
    delay(delayTime); //delay based on the current potentiometer setting for the vibration motor controller
}


}

void vibrationPulse(pulseMode) {                           //Pulse the vibration mode according to the speed of the vibration 
      // we will have to continually monitor the state of the pulse mode button so that if it is pressed again the while loop is broken

while (pulseMode = 1){
  vibrationRampUp(inData[0]);
  vibrationRampDown(inData[0]);
  //TODO: Check pulseMode button status here during each loop
  if(inData[8] == 0) {. //TODO: Change the inData array to match whatever button or condition that we use to enable pulseMode
    pulseMode = 0;
  }
}

}

void showSerial() {
  Serial.print("Masterboard Status: ");
  if (flag == 0) {                                // deadman (no connection) switch to stop motors
    Serial.println("Controller disconnected. (Debugging)");
  }
  else if (flag == 1) {
    Serial.println("Controller connected. (Debugging)");
  }
  Serial.print("Vibration Motor:");
  Serial.print(inData[0]);
  Serial.print(" / ");
  Serial.print("Rotation Motor:");
  Serial.print(inData[1]);
  Serial.print(" / ");
  Serial.print("Vibration Max Button :");
  Serial.print(button0);
  Serial.print(" / ");
  Serial.print("Ramp Mode Button :");
  Serial.print(rampModeButton);
  Serial.print(" / ");
  Serial.print("Ramp Mode Flag Flag:");
  Serial.print(rampModeButtonflag);
  Serial.print(" / ");
  Serial.print("Ramp Mode:");
  Serial.print(rampmode);

  Serial.println();
  Serial.println();
}