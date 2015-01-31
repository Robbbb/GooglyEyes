// This is the main loop for the Googly Eyes.
// It was written with reference to the code of several Instructablers. Randy,
// Amanda, and more.

// See the instructable at: http://www.instructables.com/id/E6APAUWI5GN9L8M/

// #include <Firmata.h>  // Easiest most reilable way to move values from
// computer to Arduino //Not used at the moment
#include <AccelStepper.h>  // A fantstic absolute and non-blocking stepper motor control library
#include <Servo.h>  // Used to send servo-type signals to the DC motor controller
#include <Wire.h>   //Wire is a library for I2C communiction. We are using it to send data between arduinos.

const boolean isLeftEye = true;  // The two eyes are different.
// Left Eye is master, always
const boolean isMaster = isLeftEye;  // The two eyes are different.

AccelStepper stepper(AccelStepper::FULL2WIRE,
                     12,
                     13);  // initialize a two-wire stepper on pins 12 and 13
Servo rotationalMotorController;  // initialize our "servo", actually a Parralx

const int pwmA = 3;  // On Motor Shield, PWM control pins
const int pwmB = 11;
const int brakeA = 8;  // On Motor Shield, Brake Control pins
const int brakeB = 9;
const int hiLimitPin = A5;  // Photodiode linear limit switch pins. Active low.
const int centerLimitPin = A3;
const int lowLimitPin = A4;
const int stepperSpeed =
    600;  // trial and error detemrined this to be the best speed.
const int stepsPerRotation = 200;  // Our stepper is a 1.8 degree per step motor
const long MAX_WAIT_MS =
    10000;  // number of milliseconds after which to give up on a move command

int hiLimitValue;      // state of linear limit switch
int centerLimitValue;  // state of linear limit switch
int lowLimitValue;     // state of linear limit switch

long hiLimitSteps = 1600;  // maximum linear steps from origin we can reasonably
// expect before tripping a limit switch
long loLimitSteps = -1600;

long linearStepperGoal = 0;
long lastR;
int lastTheta;

int rotaryPositionGoal = 6;
int rotaryPositionMillis;
int lastKnownSpinPosition = -1;    // was 6 a moment ago
int lastKnownLateralPosition = 0;  // always known!

int spinThetaGoal = 6;
boolean thetaGoalReached = false;

void setup() {
  // firmataFakeAnalogSetup();
  Serial.begin(9600);
  // Serial.println("instructable at
  // instructables.com/id/E6APAUWI5GN9L8M");//");//
  // Serial.println("Current arduino code at github.com/Robbbb/GooglyEyes");
  // Serial.println();
  // Serial.println("\t\t( o ) ( o )");
  // Serial.println();

  if (isLeftEye)
    Serial.print("Left Eye. ");
  else
    Serial.print("Right Eye. ");

  if (isMaster)
    i2cMasterSetup();
  else
    i2cSlaveSetup();

  //  linearSetup();
  //  linearHome();
  //  rotarySetup();
  //  rotaryHome();
  //  scream(1);
  // pickRoutine();
  // scream(2);
  // testIntToString();
}

void loop() {
  if (isMaster) {
    // i2cMasterTest();
    danceRandom(1000);
  } else  // is slave
  {
    parsei2cString();
    moveTo(linearStepperGoal, spinThetaGoal);
  }
  // // danceRandom(10000);
  // // blockingGoHome();
  // // delay(500);
  // // scream(4);
  // delay(100);
}

String intToString(int intToConvert) {
  // Takes an intToConvert and formats it into a robust and unabiguous signed
  // string of constant lenght range is -32,768 to +32,768
  String stringToReturn = "";
  if (intToConvert >= 0)
    stringToReturn += '+';
  else
    stringToReturn += '-';
  int magnitude = abs(intToConvert);
  int len = 1;
  int disposableMagnitude = magnitude;
  while (disposableMagnitude >= 10) {
    len++;
    disposableMagnitude /= 10;
  }
  int maxDigitQty = 5;
  int leadingZeroQty = maxDigitQty - len;
  for (int i = 0; i < leadingZeroQty; i++) {
    stringToReturn += '0';
  }
  stringToReturn += magnitude;
  // Serial.println(stringToReturn);
  return stringToReturn;
}

void testIntToString() {
  Serial.println("Testing intToString...");
  Serial.println((intToString(-32000) == "-32000"));
  Serial.println(intToString(32000) == "+32000");
  Serial.println(intToString(-2) == "-00002");
  Serial.println(intToString(2) == "+00002");
  Serial.println(intToString(1) == "+00001");
  Serial.println(intToString(0) == "+00000");
  Serial.println("if there are no zeros, passed test!");
}

void pickRoutine() {
  // picks between the (blocking) eye movement routines and executes them
  int selectionQty = 3;
  int selection = random(selectionQty);
  selection = 2;
  int duration = random(2000, 5000);
  switch (selection) {
    case 1:
      danceRandom(duration);
      break;
    case 2:
      rollEyesOver(duration);
      break;
      // default:
  }
}

void rollEyesOver(int duration) {
  // When called, quickly rolls the eyes over the horizon
  // TO DO: FIX IT
  while (!moveTo(hiLimitSteps, 6)) {
  }
  while (!moveTo(hiLimitSteps, 11)) {
  }
  while (!moveTo(hiLimitSteps, 14)) {
  }
  while (!moveTo(0, 14)) {
  }
  while (!moveTo(0, 6)) {
  }
}

void danceRandom(unsigned long duration) {
  // Dances to a random coordinate and for random durations and does so for a
  // random duration
  // Serial.println("danceRandom called...");
  unsigned long start = millis();
  long r = 11111;
  int theta = 11;
  bool gotThere = true;
  while (millis() - start < duration) {
    if (gotThere) {
      r = random(int(hiLimitSteps * 0.2), hiLimitSteps);
      theta = random(0, 8);
    }
    gotThere = moveTo(r, theta);
  }
}

void blockingGoHome() {
  while (!moveTo(0, 6)) {
  }
}

boolean moveTo(long r, int theta) {
  // Returns true when goalsd are reached and false upon failure
  // boolean debug= false;
  // if(debug){
  //   Serial.print("moving to r:\t");
  //   Serial.print(r);
  //   Serial.print("\tand theta:\t");
  //   Serial.println(theta);
  // }

  boolean newR = (r != lastR);  // new if different
  boolean newTheta = (theta != lastTheta);
  lastR = r;
  lastTheta = theta;
  if (isMaster)  //&& (newR || newTheta))
  {
    sendPositionToI2c(theta, int(r));  // Send same instructions to other eye
  }

  if (!updateLinearLimits()) {  // returns false when limit is tripped
    linearHome();  // if the carriage hits a limit switch, it is not properly
                   // zeroed and should be zero'd again.
  }
  stepper.moveTo(r);
  stepper.setSpeed(stepperSpeed);

  stepper.runSpeedToPosition();
  boolean hitR = stepper.currentPosition() == r;
  if (spinTo(theta)) {
    thetaGoalReached = true;
  }
  return hitR && thetaGoalReached;
}

void scream(int beepQty) {
  // Beep the specified number of times using the stepper motor as a speaker
  // Be careful, it is a blocking function.
  int duration = beepQty * 500;
  int freq = 500;
  for (int i = 0; i < beepQty; ++i) {
    tone(12, freq);
    delay(duration / beepQty);
    noTone(12);
    delay(duration / beepQty);
  }
}

/// Firmata Stuff

// Uncomment and edit if you are controlling from a computer
/*

 void firmataLoop(){
 while (Firmata.available()) {  // if it gets a command from firmata, it does
 what it is supposed to do
 Firmata.processInput();
 stepper.moveTo(linearStepperGoal);
 }

 }

 void analogWriteCallback(byte pin, int value) {
 int imaginaryPinUsedToRecieveScaledLinearGoal = 2;
 int imaginaryPinUsedToRecieveRotaryGoal = 3;

 if (pin == imaginaryPinUsedToRecieveScaledLinearGoal) {
 linearStepperGoal = map(value, 0, 360, loLimitSteps, hiLimitSteps);
 }
 if (pin == imaginaryPinUsedToRecieveRotaryGoal) {
 rotaryPositionGoal = value;
 }
 }

 void firmataFakeAnalogSetup() {
 Firmata.setFirmwareVersion(0, 2);
 Firmata.attach(ANALOG_MESSAGE, analogWriteCallback);
 Firmata.begin(57600);
 Serial.println("\tFirmata Begun.");
 int imaginaryPinUsedToDifferentiateTheEyes = 0;
 Firmata.sendAnalog(imaginaryPinUsedToDifferentiateTheEyes, isLeftEye);
 }
 */
