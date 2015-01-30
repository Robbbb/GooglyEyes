
void i2cSlaveSetup(){
  // if master
  Serial.print("i2cSlaveSetup...");
  Wire.begin(4); // join i2c bus (address optional for master)
  Wire.onReceive(receiveEvent); // register event
  Serial.println("Complete.");


}

void i2cMasterSetup(){
  //  testIntToString();
  Serial.print("i2cMasterSetup...");
  Wire.begin();
  Serial.println("Complete.");

}

void i2cMasterTest(){
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

// void i2cSlaveTest(){
//       Serial.print("i2cSlaveTest...");
//   int delayAmt = 500;

//   delay(delayAmt);

//   delay(delayAmt);

//   delay(delayAmt);
//       Serial.println("Complete.");
// }


//arduion
void sendPositionToI2c(int thetaToSend, int rToSend){
  //builds a constant lenght string containing the theta and radius params
  //format style: "T+00016R-15555X"
  String stringToSend = "";
  stringToSend += "T";
  stringToSend += intToString(thetaToSend);
  stringToSend += "R";
  stringToSend += intToString(rToSend);
  stringToSend += "X";
  Serial.println(stringToSend);
  char charBuf[15];
  stringToSend.toCharArray(charBuf, 15);
  Wire.write(charBuf);              // sends one byte  
  // Wire.write('T+00015R+00999X');              // sends one byte  
  //              T+000000R+000000X

  Wire.endTransmission();    // stop transmitting
  Serial.println("sendPositionToI2c executed");

}

String intToString(int integer){
  //Takes an integer and formats it into a robust and unabiguous signed string of constant lenght range is -32,768-32,768
  String message = "";
  if(integer >= 0) message+= "+";
  else message+= "-";
  int magnitude = abs(integer);
  int len = 1;
  int disposableMagnitude = magnitude;
  while (disposableMagnitude >= 10){
    len ++;
    disposableMagnitude /= 10;
  }
  int maxDigitQty = 5;
  int leadingZeroQty = maxDigitQty - len;
  for(int i=0; i<leadingZeroQty; i++){
    message+="0";
  }
  message+=magnitude;
  return message;
}

void testIntToString(){
  Serial.println("Testing intToString...");
  Serial.print("intToString(-32000)  ");
  Serial.println(intToString(-32000));
  Serial.println(intToString(-32000)=="-32000");
  Serial.println(intToString(32000)=="+32000");
  Serial.println(intToString(-2)=="-00002");
  Serial.println(intToString(2)=="+00002");
  Serial.println(intToString(0)=="+00000");

}
String eyeCoordStringCandidate = "";


void receiveEvent(int howMany)
{
  eyeCoordStringCandidate = "";
  //a blocking function that collects all the data from the other board
  while( Wire.available()) // loop through all 
  {
    char inChar = Wire.read(); // receive byte as a character
    eyeCoordStringCandidate += inChar;
  }
  Serial.println(eyeCoordStringCandidate);

}


void parsei2cString(){
  //error checks global eyeCoordStringCandidate and sets variables if it is valid
  int XXxtheta;
  int XXxr;

  String eyeCoordString = "";
  if (eyeCoordStringCandidate.length() < 15) return;
  boolean rightLegnth = eyeCoordStringCandidate.length() ==15;
  boolean rightBegin = eyeCoordStringCandidate.charAt(0) == 'T';//T for Theta
  boolean rightEnd = eyeCoordStringCandidate.charAt(7) == 'R';  //R for radius
  if (rightLegnth && rightBegin && rightEnd){
    //looks legit, lets use it
    Serial.println("eyeCoordStringCandidate passed all tests!");
    eyeCoordString = eyeCoordStringCandidate;
    eyeCoordStringCandidate= "";//blank the old one
    XXxtheta = eyeCoordString.substring(1,1+6).toInt();
    XXxr =  eyeCoordString.substring(8,8+6).toInt();
    Serial.print("theta: ");
    Serial.println(XXxtheta);
    Serial.print("r: ");
    Serial.println(XXxr);
  }
  else{
    //what went wrong?
    Serial.println(eyeCoordStringCandidate);
    eyeCoordStringCandidate= "";//blank the old one
    Serial.print(rightLegnth);
    Serial.print(rightBegin);
    Serial.print(rightEnd);
    Serial.print("eyeCoordStringCandidate.length():  ");
    Serial.println(eyeCoordStringCandidate.length());
    Serial.print("eyeCoordStringCandidate.charAt(0):  ");
    Serial.println(eyeCoordStringCandidate.charAt(0));
    Serial.print("eyeCoordStringCandidate.charAt(7): ");
    Serial.println(eyeCoordStringCandidate.charAt(7));

    Serial.println("eyeCoordStringCandidate Failed a test!");
  }

  //  Serial.print("succsess receive of entire string: ");
  //  //  Serial.println(eyeCoordString);

}














