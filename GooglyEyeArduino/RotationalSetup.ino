int sensorPins[] = {//the pins nos. for the hall effect sensor array 
  2,4,5,6,7};

String lookupTable[16];//truth table for psuedo binary to position

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
  //By Amanda, moderately modified
  // if (stepper.currentPosition()>1)
  // {
  //   lastKnownLateralPosition = 1;
  // } 
  // else if (stepper.currentPosition()<1)
  // {
  //   lastKnownLateralPosition = -1;
  // } 
  // else{
  //   lastKnownLateralPosition = 0;
  // }

  //identifies theposition and returns it. If unknown, returns -1. Expected bounds are -1 to 16

  String sensorData = "";//empty string to be filled with binary positions
  for (int i = 0; i < 5; i++) {
    // HallEffectControl *control = _hallSensorControllers[i];
    boolean hit =  digitalRead(sensorPins[i]);
    if (hit) {
      sensorData = sensorData + "0";
    } 
    else {
      sensorData = sensorData + "1";
    }
  }
  //  Serial.print("\tRaw:  "); Serial.println(sensorData);
  if (sensorData.compareTo("00000")==0) {
    return -1;//no hits, floating
  } 
  else {//hits!!
    for (int i = 0; i<16; i++) {
      if (sensorData.equals(lookupTable[i])) {
        if (!checkForFalseHits(i)) continue;//back to the top, incrementing i?
        lastKnownSpinPosition = i;
        // if (lastKnownLateralPosition==-1){//This is complicated polar math we can ignore
        //   int oppPosition = i+8;//um mod much?
        //   if (oppPosition>15) return oppPosition-16;
        //   return oppPosition;
        // } 
        // else if (lastKnownLateralPosition==1){
        return i;
        //}
      }
    }
  }
  return -1;//no hits
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
  //Store the physical pattern of the magnets in our wacky encoder

  lookupTable[0]  = "10100";//   00 - OXXXX
  lookupTable[1]  = "11000";//   01 - OOXXX
  lookupTable[2]  = "10000";//   02 - OXOXX
  lookupTable[3]  = "11111";//   03 - OXXOX
  lookupTable[4]  = "11110";//   04 - OXXXO
  lookupTable[5]  = "11101";//  105 - OOOXX
  lookupTable[6]  = "11011";//   06 - OXOOX//HOME
  lookupTable[7]  = "10111";//   07 - OXXOO
  lookupTable[8]  = "11010";//   08 - OOXXO
  lookupTable[9]  = "10101";//   09 - OXOXO
  lookupTable[10] = "11001";//   10 - OOXOX
  lookupTable[11] = "10011";//   11 - OXOOO
  lookupTable[12] = "00110";//   12 - OOXOO
  lookupTable[13] = "01100";//   13 - OOOXO
  lookupTable[14] = "10001";//   14 - OOOOX//AntiHome== -Home
  lookupTable[15] = "10010";//   15 - XOOOX


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









