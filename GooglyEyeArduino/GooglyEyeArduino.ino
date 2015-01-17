// This is the main loop for the Googly Eyes.
// It was written with reference to the code of several Instructablers. Randy, Amanda, and more.

// See the instructable at:

// #include <Firmata.h>  // Easiest most reilable way to move values from computer to Arduino
#include <AccelStepper.h>  // A great absolute and non-blocking stepper contro library
#include <Servo.h>  // Used to send servo-type signals to the DC motor controller

AccelStepper stepper(AccelStepper::FULL2WIRE, 12, 13);  // initialize a two-wire stepper on pins 12 and 13
Servo rotationalMotorController;  // initialize our "servo", actually a Parralx

const boolean isLeftEye = true;  // The two eyes are different.

const int pwmA = 3;  // On Motor Shield, PWM control pins
const int pwmB = 11;
const int brakeA = 8;  // On Motor Shield, Brake Control pins
const int brakeB = 9;

const int hiLimitPin = A5;  // Photodiode linear limit switch pins. Active low.
const int centerLimitPin = A3;
const int lowLimitPin = A4;

const int stepperSpeed = 600;  // trial and error detemrined this to be the best speed.

const int stepsPerRotation = 200;  // Our stepper is a 1.8 degree per step motor

int hiLimitValue;  // state of linear limit switch
int centerLimitValue;  // state of linear limit switch
int lowLimitValue;  // state of linear limit switch

long hiLimitSteps = 1600;  // maximum linear steps from origin we can reasonably expect before tripping a limit switch
long loLimitSteps = -1600;

long linearStepperGoal = 0;

int rotaryPositionGoal = 6;
int rotaryPositionMillis;
int lastKnownSpinPosition = -1;  // was 6 a moment ago
int lastKnownLateralPosition = 0;  // always known!

void setup() {
  // firmataFakeAnalogSetup();
  linearSetup();
  linearHome();
  rotarySetup();
  rotaryHome();
}

void loop() {
  if (!updateLinearLimits()) {  // returns false when limit is tripped
    linearHome();  // if the carriage hits a limit switch, it is not properly zeroed and should be zero'd again. 
  }
  // while (Firmata.available()) {  // if it gets a command from firmata, it does what it is supposed to do
  //   Firmata.processInput();
  //   stepper.moveTo(linearStepperGoal);
  // }
  // LINEAR
  stepper.moveTo(hiLimitSteps);
  stepper.setSpeed(stepperSpeed);
  stepper.runSpeedToPosition();
  // // ROTATIONAL
  while(!spinTo(4)) {}
}

///Firmata Stuff!

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

// void firmataFakeAnalogSetup() {
//   Firmata.setFirmwareVersion(0, 2);
//   Firmata.attach(ANALOG_MESSAGE, analogWriteCallback);
//   Firmata.begin(57600);
//   Serial.println("\tFirmata Begun.");
//   int imaginaryPinUsedToDifferentiateTheEyes = 0;
//   Firmata.sendAnalog(imaginaryPinUsedToDifferentiateTheEyes, isLeftEye);
// }
