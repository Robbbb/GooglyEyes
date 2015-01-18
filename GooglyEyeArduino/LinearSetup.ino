
void linearSetup(){
  //Runs once. Prepares motor driver to move linear stepper.
  pinMode(pwmA, OUTPUT);//init the 4 motor control pins for the shield to be outputs
  pinMode(pwmB, OUTPUT);
  pinMode(brakeA, OUTPUT);
  pinMode(brakeB, OUTPUT);

  pinMode(hiLimitPin, INPUT);//init the three breakbeam limit switches as inputs
  pinMode(centerLimitPin, INPUT);
  pinMode(lowLimitPin, INPUT);

  enableStepper();
  digitalWrite(brakeA, LOW);//We are using AccelStepper, so we just disable the brakes
  digitalWrite(brakeB, LOW);

  //Out motor is a Intelligent Motion Systems M2-2220 Stepper NEMA-23 24v
  //motor datasheet: http://www.scopecraft.com/Images/NEMA%2023H.pdf
  //our rod is a 1" per rev helical lead screw
  stepper.setMaxSpeed(stepperSpeed); //this has no effect other than preventing overflow
  stepper.setAcceleration(200);
}
const int crossedBeam = 0;//the breakbeams are active low

bool updateLinearLimits(){
  //Returns True if it is in a safe place and false otherwise
  //Polls the three photo diodes and detemrines position of pupil
  bool debugLinearLimits = false;//test the sensors if you are paranoid.

  hiLimitValue = digitalRead(hiLimitPin);
  centerLimitValue = digitalRead(centerLimitPin);
  lowLimitValue= digitalRead(lowLimitPin);

  if (debugLinearLimits == true){
    Serial.print("Limits Updated:\t");
    Serial.print("hiLimitValue:\t");
    Serial.print(hiLimitValue);
    Serial.print("\t centerLimitValue:\t");
    Serial.print(centerLimitValue);
    Serial.print("\tlowLimitValue:\t");
    Serial.println(lowLimitValue);

  }
  if (centerLimitValue == crossedBeam)return true;
  else if (lowLimitValue == crossedBeam)return false;
  else if (hiLimitValue == crossedBeam)return false;
  else return true;
}

void linearHome(){
  Serial.print("Homing linear axis...");
  stepper.moveTo(0);
  stepper.setSpeed( 1 * stepperSpeed);

  // This blocking function sends the linear actulator to its home/origin
  
  while (true) {  //if it isn't centered already...
    updateLinearLimits();
    if (centerLimitValue == crossedBeam) {
      break;
    }

    bounceAtLimits();  // changes the speed to -thespeed if it hits a limit
    stepper.runSpeed();
  }

  stepper.setCurrentPosition(0); // This is your home now, stepper
  Serial.println("Succeeded! Pupil at origin.");
}

void bounceAtLimits(){
  //changes the speed to -thespeed if it hits a limit
  while(lowLimitValue == crossedBeam){//if it is under, run it 'up'
    enableStepper();
    updateLinearLimits();
    stepper.setSpeed( 1 * stepperSpeed);
    stepper.runSpeed();
  }
  while(hiLimitValue == crossedBeam){//if it is over, run it 'down'
    enableStepper();
    updateLinearLimits();
    stepper.setSpeed( -1 * stepperSpeed);
    stepper.runSpeed();
  }
}






void enableStepper(){
  digitalWrite(pwmA, HIGH);//We are using AccelStepper, so we just enable at full tilt(5v)
  digitalWrite(pwmB, HIGH);
}
void disableStepper(){
  //  stepper.stop();
  digitalWrite(pwmA, LOW);//We are using AccelStepper, so we just enable at full tilt(5v)
  digitalWrite(pwmB, LOW);

}
/*

 All this stuff was a failed attempt I am not ready to abandon
 
 
 void sendLinearHome(){
 int home = 0;
 linearMoveTo(home);
 }
 
 void scream(int duration){
 digitalWrite(pwmA, HIGH);//We are using AccelStepper, so we just enable at full tilt(5v)
 digitalWrite(pwmA, HIGH);
 //  digitalWrite(pwmB, HIGH);
 tone(pwmB, 5000, duration);
 delay(duration);
 digitalWrite(pwmB, HIGH);
 delay(duration);
 }
 
 
 void findAndPrintLimits(){
 Serial.println("Finding True Limits...");
 //This blocking function sends the linear actulator to its extents after it has been homed
 // while(updateLinearLimits()!=false){//this turns true as soon as it leaves the gate....
 while(linearMoveTo(hiLimitSteps)){
 }
 
 Serial.println("break   while (linearMoveTo(hiLimitSteps)){");
 delay(2000);
 sendLinearHome();
 Serial.println("sent sendLinearHome");
 delay(2000);
 // while(updateLinearLimits()!=false){
 while(linearMoveTo(loLimitSteps)){
 }
 
 sendLinearHome();
 Serial.println("Succeeded! Bounds Found.");
 }
 
 boolean linearMoveTo(int target){
 boolean debug = false;
 if (debug){
 Serial.print("target:\t");
 Serial.print(target);
 Serial.print("\tstepper.currentPosition():\t");
 Serial.println(stepper.currentPosition());
 }
 //safely quickly silently moves the (homed) track to a goal. Target is in steps, -/+2400
 //  if (updateLinearLimits()==false){//if an edge switch is triped
 //    int collison = stepper.currentPosition();
 //    if (collison>0)
 //    {
 //      hiLimitSteps = collison;
 //      Serial.print("\thiLimitSteps:\t");
 //      Serial.println(hiLimitSteps);
 //    }
 //    else if (collison<0)
 //    {
 //      loLimitSteps = stepper.currentPosition();
 //      Serial.print("\tloLimitSteps:\t");
 //      Serial.println(loLimitSteps);
 //    }
 //    else{
 //      Serial.println("IMPOSSIBLE ERROR");
 //    }
 //
 //    Serial.print("Hit Limits at:\t");
 //    Serial.print(collison);
 //    Serial.print("\twhile heading toward:\t");
 //    Serial.println(target);
 //    //    while it thinks it is above its limit, keep it off. As soon as it thinks it is back at or below its limit,  
 //  }
 
 if (hiLimitSteps<= stepper.currentPosition() || loLimitSteps >= stepper.currentPosition() ){//if it wants to go furhter than it can, shut it down until its goal is reaonable
 disableStepper();//this shuts off power. the library is none the wiser
 Serial.println("disableStepper");
 return false;
 }
 else{
 enableStepper();//if the goal goes back into the green, re-enable it
 Serial.println("enableStepper");
 // return false;
 //  else{
 stepper.moveTo(target);
 stepper.setSpeed(stepperSpeed);
 stepper.runSpeedToPosition();
 return true;
 //  }
 }
 }
 */





