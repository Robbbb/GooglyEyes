
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








