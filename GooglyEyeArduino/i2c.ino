
void i2cSlaveSetup(){
  Serial.print("i2cSlaveSetup...");
  Wire.begin(4); // join i2c bus (address optional for master)
  Wire.onReceive(receiveEvent); // register event
  Serial.println("Complete.");
}

void i2cMasterSetup(){
  Serial.print("i2cMasterSetup...");
  delay(1000);
  Wire.begin();
  Serial.println("Complete.");
}


// String intToStringBuffer;  

void sendPositionToI2c(int thetaToSend, int rToSend){
  //builds a constant lenght string containing the theta and radius params
  //format style: "T+00016R-15555X"
  boolean debug = true;
  if(debug){
    Serial.print("moving to rToSend:\t");
    Serial.print(rToSend);
    Serial.print("\tand thetaToSend:\t");
    Serial.println(thetaToSend);
  }
  String stringToSend = "";
  stringToSend += "T";
  // intToString(thetaToSend);
  stringToSend += intToString(thetaToSend);
  stringToSend += "R";
  stringToSend +=   intToString(rToSend);
  stringToSend += "X";
  Serial.print("Attempting to send over i2c:\t");
  Serial.println(stringToSend);
  char charBuf[15];
  stringToSend.toCharArray(charBuf, 15);

  Wire.beginTransmission(4); // transmit to device #4
  Wire.write(charBuf);              // sends 15 byte s 
  Wire.endTransmission();    // stop transmitting

  Serial.println("sendPositionToI2c executed");
}

String eyeCoordStringCandidate = "";

void receiveEvent(int howMany)
//Automatically called by Wire library when a i2c packet is recieved
{
  eyeCoordStringCandidate = "";
  //a blocking function that collects all the data from the other board
  while( Wire.available()) // loop through all 
  {
    char inChar = Wire.read(); // receive byte as a character
    eyeCoordStringCandidate += inChar;
  }
}

void parsei2cString(){
  //error checks global eyeCoordStringCandidate and sets variables if it is valid
  // Serial.println("parsei2cString...");
  if(eyeCoordStringCandidate.length()==0)return; //quit if it is blank
  Serial.print("parsing: ");
  Serial.println(eyeCoordStringCandidate);
  String eyeCoordString = "";
  boolean rightLegnth = eyeCoordStringCandidate.length() ==14;
  boolean rightBegin = eyeCoordStringCandidate.charAt(0) == 'T';//T for Theta
  boolean rightEnd = eyeCoordStringCandidate.charAt(7) == 'R';  //R for radius
  if (rightLegnth && rightBegin && rightEnd){
    // Serial.println("eyeCoordStringCandidate passed all tests!");
    eyeCoordString = eyeCoordStringCandidate;
    eyeCoordStringCandidate= "";//blank the old one
    spinThetaGoal = eyeCoordString.substring(1,1+6).toInt();
    linearStepperGoal =  eyeCoordString.substring(8,8+6).toInt();
    Serial.print("theta: ");
    Serial.println(spinThetaGoal);
    Serial.print("r: ");
    Serial.println(linearStepperGoal);
  }
  else{
    Serial.println("Recieved bad string over i2c...");
    Serial.println(eyeCoordStringCandidate);
    Serial.print("eyeCoordStringCandidate.length():  ");
    Serial.println(eyeCoordStringCandidate.length());
    Serial.print("eyeCoordStringCandidate.charAt(0):  ");
    Serial.println(eyeCoordStringCandidate.charAt(0));
    Serial.print("eyeCoordStringCandidate.charAt(7): ");
    Serial.println(eyeCoordStringCandidate.charAt(7));
    Serial.println("eyeCoordStringCandidate Failed a test!");
  }
}








/*
String intToString(int intToConvert){
  //Takes an intToConvert and formats it into a robust and unabiguous signed string of constant lenght range is -32,768 to +32,768
  String stringToReturn = "";
  if(intToConvert >= 0) stringToReturn += '+';
  else stringToReturn+= '-';
  int magnitude = abs(intToConvert);
  int len = 1;
  int disposableMagnitude = magnitude;
  while (disposableMagnitude >= 10){
    len ++;
    disposableMagnitude /= 10;
  }
  int maxDigitQty = 5;
  int leadingZeroQty = maxDigitQty - len;
  for(int i=0; i<leadingZeroQty; i++){
    stringToReturn+='0';
  }
  stringToReturn += magnitude;
  Serial.println(stringToReturn);
  // intToStringBuffer =  stringToReturn;
  return stringToReturn;
}


void testIntToString(){
  //Ensure the int to string function behaves predictably
  Serial.println("Testing intToString...");
  Serial.print("intToString(-32000)  ");
  // intToString(-32000);
  // Serial.println(intToStringBuffer);

  Serial.println(intToString(-32000)=="-32000");
  Serial.println(intToString(32000));//=="+32000");
  Serial.println(intToString(-2));//=="-00002");
  Serial.println(intToString(2));//=="+00002");
  Serial.println(intToString(0));//=="+00000");
  Serial.println("end test intToString.");

}
*/
void i2cMasterTest(){
  //Sends a trio of example commands to tet the protocol and the parsing
  Serial.println("i2cMasterTest...");
  delay(3000);
  Serial.println("begin");
  int delayAmt = 500;
  Serial.println("sendPositionToI2c(0,0);...");
  sendPositionToI2c(0,0);
  delay(delayAmt);
  Serial.println("sendPositionToI2c(16,1400);");
  sendPositionToI2c(16,1400);
  delay(delayAmt);
  Serial.println("sendPositionToI2c(8,-1400);");
  sendPositionToI2c(8,-1400);
  delay(delayAmt);
  Serial.println("Complete.");
}

