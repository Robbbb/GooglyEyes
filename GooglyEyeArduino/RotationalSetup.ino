int sensorPins[] = {//the pins nos. for the hall effect sensor array 
  2,4,5,6,7};

const byte lookupTable[] = {
  0b00101,
  0b00011,
  0b00001,
  0b11111,
  0b01111,
  0b10111,
  0b11011,  // (Home)
  0b11101,
  0b01011,
  0b10101,
  0b10011,
  0b11001,
  0b00011,
  0b00011,
  0b10001,  // (AntiHome)
  0b01001,
};

void   rotarySetup(){
  hallArraySetup();
  rotationalMotorController.attach(10);  // attaches the servo on pin 9 to the servo object 
  rotationalMotorController.writeMicroseconds(1500);    
  //  testCleanRotaryGoal();
}

void rotaryHome(){
  //A blocking function that brings the rotary thing home
  Serial.print("Homing roatry axis...");

  //  while(HallArrayIdentifier()!= 6){
  //    spinClockwise();
  while(!spinTo(6)){
  }  

  spinStop();
  // rotaryPositionMillis = 0;  
  Serial.println("Succeeded! rotary at origin.");
}
int cleanRotaryGoal(int goal, int reality){
  //if the fastest route is backwards, spit out the fastest route
  //remeber pupil, nearest exit may be behind you. 
  //could be doen with % but I couldn't figure it out
  int totalPositions = 16;
  int error = goal-reality;
  if (error >(totalPositions/2)){
    error=error-totalPositions  ;
  }
  if (error <(-totalPositions/2)){
    error=error+totalPositions  ;
  }
  return error;
}


boolean spinTo(int target){
  HallArrayIdentifier();//detect hits on hall array
  if (lastKnownSpinPosition == -1)spinClockwise();//if it doesn't know where it is, spin it
  if(lastKnownSpinPosition == target){
    spinStop();
    return true;
  }
  int totalPositions = 16;

  int fastestRotaryRoute = cleanRotaryGoal(target,lastKnownSpinPosition);
  if(fastestRotaryRoute == 0){
    spinStop();
    return true;
  }

  spinAt(fastestRotaryRoute);
  //  if(fastestRotaryRoute<1){//neg numbers
  //    spinCounterClockwise();
  //  }
  //  else if(fastestRotaryRoute>1){
  //
  //    spinClockwise();
  //  }

  //  int timeZero = millis();
  //  while(hallArrayUpdate() == false){
  //  }
  // int timeOne = millis();
  // int spinDuration = timeOne-timeZero;
  // Serial.print("Spun once at: ");
  // Serial.print(speed);
  // Serial.print("\ttook: ");
  // Serial.println(spinDuration);

  //  Serial.print("\tposition: ");
  //  Serial.print(lastKnownSpinPosition);
  //  Serial.print("\ttarget: ");
  //  Serial.print(target);
  //  Serial.print("\troute: ");
  //  Serial.println(fastestRotaryRoute);
  return false;
}



int rotaryStopMillis = 1500; //the pulse duration that stops the motor
int rotarySpindesiredMagnitude = 1000;//the greatest allowed distance from stop
int rotarySpinMaximumMagnitude = 500;//the greatest allowed distance from stop

int rotaryDesiredSpeed = 200;
int rotaryClockwiseMillis = rotaryStopMillis +rotaryDesiredSpeed;
int rotaryCounterClockwiseMillis = rotaryStopMillis -rotaryDesiredSpeed;

void spinAt(int speed){

  //  switch (speed) {
  //  case 0:
  //    spinStop();
  //    break;
  //  case 1://if we are close slow the heck down
  //    rotationalMotorController.writeMicroseconds(rotaryStopMillis+100);
  //    break;
  //    case -1://if we are close slow the heck down
  //    rotationalMotorController.writeMicroseconds(rotaryStopMillis-100);
  //    break;
  //  default: 
  int addend = int(map(speed,-8,8, -rotarySpindesiredMagnitude, rotarySpindesiredMagnitude));
  addend = constrain(addend,-rotarySpinMaximumMagnitude,rotarySpinMaximumMagnitude);
  rotationalMotorController.writeMicroseconds(rotaryStopMillis+addend);

}
void spinClockwise(){
  rotationalMotorController.writeMicroseconds(rotaryClockwiseMillis);
}
void spinCounterClockwise(){
  rotationalMotorController.writeMicroseconds(rotaryCounterClockwiseMillis);
}
void spinStop(){
  rotationalMotorController.writeMicroseconds(rotaryStopMillis);
}

int HallArrayIdentifier(){
  byte reading = 0;
  for (int i = 0; i < 5; i++) {
    int pin = sensorPins[i];
    if (digitalRead(pin) == LOW) {
      reading |= (1 << i);
    }
  }

  if (reading == 0) {
    return -1;  //no hits, floating
  } 

  for (int i = 0; i < 16; i++) {
    if (reading != lookupTable[i]) {
      continue;
    }
    if (!checkForFalseHits(i)) {
      continue;
    }

    lastKnownSpinPosition = i;
    return i;
  }

  return -1;  //no hits
}


boolean checkForFalseHits(int spinPositionCandidate){
  if (lastKnownSpinPosition == -1) return true;//Should only be the case at boot
  int jumpSize =abs(cleanRotaryGoal(spinPositionCandidate,lastKnownSpinPosition));
  if (jumpSize>1)
  {
    return false;
  }
  // if suspected index found, make sure it is in reason
  //while eye is spinning, sometimes not all hall sensors in array have settled on state at the same time, check that new vals make sense

    // if (lastKnownSpinPosition==16) return true;//coming from center
  // if (abs(lastKnownSpinPosition-spinPositionCandidate) >= 1) return true;
  //delta cand+known should be 1 or less
  // if (abs(lastKnownSpinPosition-spinPositionCandidate) == 15) return true;
  return true;
}


void hallArraySetup(){
  //The Hall effect sensors are active low, meaning the absensce of a magnet casues a zero reading
  pinMode(2, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT); 
}






boolean testCleanRotaryGoal(){
  Serial.print("testCleanRotaryGoal...");
  if (cleanRotaryGoal(0,1)       == -1)             Serial.println("(cleanRotaryGoal(0,1)       == -1) ");
  else  Serial.println("failed!");
  if (cleanRotaryGoal(1,0)       ==  1)              Serial.println("(cleanRotaryGoal(1,0)       ==  1) ");
  else  Serial.println("failed!");
  if (cleanRotaryGoal(2,1)       == 1)             Serial.println("(cleanRotaryGoal(2,1)       == -1) ");
  else  Serial.println("failed!");
  if (cleanRotaryGoal(1,2)       ==  -1)              Serial.println("(cleanRotaryGoal(1,2)       ==  1) ");
  else  Serial.println("failed!");
  if (cleanRotaryGoal(0,15)      ==  1)             Serial.println("(cleanRotaryGoal(0,15)      ==  1) ");
  else  Serial.println("failed!");
  if (cleanRotaryGoal(15,0)      == -1)            Serial.println("(cleanRotaryGoal(15,0)      == -1) ");
  else  Serial.println("failed!");
  if (abs(cleanRotaryGoal(7,15)) ==  8)        Serial.println("(abs(cleanRotaryGoal(7,15)) ==  8) ");
  else  Serial.println("failed!");
  if (abs(cleanRotaryGoal(15,7)) ==  8)        Serial.println("(abs(cleanRotaryGoal(15,7)) ==  8) ");
  else  Serial.println("failed!");
  if (abs(cleanRotaryGoal(8,0))  ==  8)         Serial.println("(abs(cleanRotaryGoal(8,0))  ==  8) ");
  else  Serial.println("failed!");
  if (abs(cleanRotaryGoal(0,8))  ==  8)         Serial.println("(abs(cleanRotaryGoal(0,8))  ==  8) ");
  else  Serial.println("failed!");
  if (abs(cleanRotaryGoal(4,12)) ==  8)        Serial.println("(abs(cleanRotaryGoal(4,12)) ==  8) ");
  else  Serial.println("failed!");
  if (abs(cleanRotaryGoal(12,4)) ==  8)        Serial.println("(abs(cleanRotaryGoal(12,4)) ==  8) ");
  else  Serial.println("failed!");
  Serial.println("Passed!");
}









