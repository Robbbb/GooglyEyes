
void i2cSlaveSetup(){
  // if master
    Wire.begin(4); // join i2c bus (address optional for master)
    Wire.onReceive(receiveEvent); // register event

}

void i2cMasterSetup(){
  Wire.begin();
}




//arduion
void sendPositionToI2c(int thetaToSend, int rToSend){
  String stringToSend = "";
  stringToSend += "T";
  stringToSend += intToString(thetaToSend);
  stringToSend += "R";
  stringToSend += intToString(rToSend);
  stringToSend += "X"
  Serial.println(stringToSend);
  Wire.beginTransmission(4); // transmit to device #4
  Wire.write(stringToSend);              // sends one byte  
  Wire.endTransmission();    // stop transmitting
// "T+00016R-15555X"

}

String intToString(int integer){
  //Takes an integer and formats it into a robust and unabiguous signed string of constant lenght range is -32,768-32,768
  String message = "";
  if(integer >= 0) message+= "+";
  else message+= "-";
  int magnitude = abs(integer);
  int len = 0;
  int disposableMagnitude = magnitude;
  while (disposableMagnitude > 0){
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
Serial.println("Testing intToString...")
  Serial.println(intToString(-32000)=="-32000");
  Serial.println(intToString(32000)=="+32000");
  Serial.println(intToString(-2)=="-00002");
  Serial.println(intToString(2)=="+00002");
  Serial.println(intToString(0)=="+00000");

}

String eyeCoordString;
String eyeCoordStringCandidate;


void receiveEvent(int howMany)
{
//"T+00016R-15555X"
  //a blocking function that collects all the date from the UNIX board
  while (wire.available()) {
    char inChar = wire.read(); // receive byte as a character
    if (inChar == 'X'){//if it is X char, store it as final
      //end found
      boolean rightLegnth = eyeCoordStringCandidate.length() ==15;
      boolean rightBegin = eyeCoordStringCandidate.charAt(0) == 'T';//T for Theta
      boolean rightEnd = eyeCoordStringCandidate.charAt(7) == 'R';  //R for radius
      if (rightLegnth && rightBegin && rightEnd){
        //looks legit, lets use it
        // Serial.println("eyeCoordStringCandidate passed all tests!");
        eyeCoordString = eyeCoordStringCandidate;
        eyeCoordStringCandidate= "";//blank the old one
        int XXxtheta = eyeCoordString.substring(1,1+6).toInt();
        int XXxr =  eyeCoordString.substring(8,8+6).toInt();
      }
      else{
        eyeCoordStringCandidate= "";//blank the old one
      }
      //      Serial.print(rightLegnth);
      //      Serial.print(rightBegin);
      //      Serial.print(rightEnd);
      //      Serial.print(rightEnd2);
      //      Serial.println("eyeCoordStringCandidate Failed a test!");

    }
    else{
      eyeCoordStringCandidate += inChar ;
    }
  }
Serial.print("succsess receive of entire string: ");
Serial.println(eyeCoordString);
Serial.print("theta: ");
Serial.println(XXxtheta);
Serial.print("r: ");
Serial.println(XXxr);
delay(5);
}




