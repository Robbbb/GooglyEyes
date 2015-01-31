// Pin 13 has an LED connected on most Arduino boards.
// give it a name:

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  //testIntToString();
  Serial.begin(9600);
  // String xx = "T+00000R+15555X";
  //         Serial.println(xx.substring(1,1+6).toInt());
  //       Serial.println(xx.substring(8,8+6).toInt());
  testIntToString();
}

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
  //Serial.println(stringToReturn);
  return   stringToReturn;
}
void testIntToString(){
Serial.println("Testing intToString...");
  Serial.println((intToString(-32000)=="-32000"));
  Serial.println(intToString(32000)=="+32000");
  Serial.println(intToString(-2)=="-00002");
  Serial.println(intToString(2)=="+00002");
  Serial.println(intToString(1)=="+00001");
  Serial.println(intToString(0)=="+00000");
  Serial.println("if there are no zeros, passed test!");

}
void loop(){}

