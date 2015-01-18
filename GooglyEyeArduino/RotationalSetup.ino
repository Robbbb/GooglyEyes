int sensorPins[] = {//the pins nos. for the hall effect sensor array 
  2,4,5,6,7};

const byte lookupTable[] = {
  //This is the truth table for the wacky encoder we have on the rotary axis
  0b00101,
  0b00011,
  0b00001,
  0b11111,
  0b01111,
  0b10111,
  0b11011,  // (Home) (for right eye)
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
  int homePosition = 6;

  while(!spinTo(homePosition)){
  }  

  spinStop();
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
  //sets new goals
  
  if (target != spinThetaGoal){
  thetaGoalReached = false;
}
  spinThetaGoal = target; 

  // spinThetaGoal
  // if spin is due, spin appropriately
  HallArrayIdentifier();//detect hits on hall array
  if (lastKnownSpinPosition == -1)spinClockwise();//if it doesn't know where it is, spin it
  if(lastKnownSpinPosition == spinThetaGoal){
    spinStop();
    return true;
  }
  int totalPositions = 16;

  int fastestRotaryRoute = cleanRotaryGoal(spinThetaGoal,lastKnownSpinPosition);
  if(fastestRotaryRoute == 0){
    spinStop();
    return true;
  }

  spinAt(fastestRotaryRoute);

  return false;
}

const int rotaryStopMillis = 1500; //the pulse duration that stops the motor
// const int rotarySpinDesiredMagnitude = 1000;//the greatest allowed distance from stop
const int rotarySpinDesiredMagnitude = 1250;//the greatest allowed distance from stop
const int rotarySpinMaximumMagnitude = 500;//the greatest allowed distance from stop

const int rotaryDesiredSpeed = 200;
const int rotaryClockwiseMillis = rotaryStopMillis +rotaryDesiredSpeed;
const int rotaryCounterClockwiseMillis = rotaryStopMillis -rotaryDesiredSpeed;

void spinAt(int speed){
//Takes a speed from -8 to 8, and does a wacky linear mapping so it is proportionally faster the further away form its goal it is.
  int addend = int(map(speed,-8,8, -rotarySpinDesiredMagnitude, rotarySpinDesiredMagnitude));
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
  //returns the rotational index of the rotational axis
  const int sensorQty = 5;
  const int positionQty = 16;
  byte reading = 0;//populate the byte with zero(s)

  for (int i = 0; i < sensorQty; i++) {
    int pin = sensorPins[i];
    if (digitalRead(pin) == LOW) {//sensors are active Low
      reading |= (1 << i);//adding 1 to the reading with bitwise magic
    }
  }

  if (reading == 0) {
    return -1;  //no hits, floating between encoder steps
  } 

  for (int i = 0; i < positionQty; i++) {
    if (reading != lookupTable[i]) {
      continue;
    }
    if (!checkForFalseHits(i)) {
      continue;
    }

    int spinPosition;

    if (isLeftEye) {//left eye was built 15 or so degrees off of the right eye.
      spinPosition = (i + 1)%16;
    } else {
    spinPosition = i;
    }
  lastKnownSpinPosition = spinPosition;
  return spinPosition;//found a real position! spit it out.
  }

  return -1;  //no hits, or sensor problem
}


boolean checkForFalseHits(int spinPositionCandidate){
  if (lastKnownSpinPosition == -1) return true;//Should only be the case at boot
  int jumpSize =abs(cleanRotaryGoal(spinPositionCandidate,lastKnownSpinPosition));
  if (jumpSize>1)
  {
    return false;
  }
  // if suspected index found, make sure it is within reason
  //while eye is spinning, sometimes not all hall sensors in array have settled on state at the same time, check that new vals make sense
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
