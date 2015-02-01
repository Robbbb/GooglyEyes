#include <Servo.h>  // Used to send servo-type signals to the DC motor controller
#include <Wire.h>   //Wire is a library for I2C communiction. We are using it to send data between arduinos.

const uint8_t I2C_ADDR = 4;

void i2cSetup(bool master) {
  Serial.print("i2cSetup...");

  if (master) {
    delay(1000);
    Wire.begin();
  } else {
    Wire.begin(I2C_ADDR);          // join i2c bus (address optional for master)
    Wire.onReceive(receiveEvent);  // register event
  }

  Serial.println("Complete.");
}

void i2cWriteShort(short data) {
  // Split 16-bit short into two 8-bit chars
  int8_t hi = (data >> 8) & 0xFF;
  int8_t lo = (data >> 0) & 0xFF;

  Wire.write(hi);
  Wire.write(lo);
}

short i2cReadShort() {
  // Reassemble two 8-bit chars into one 16-bit short
  return (Wire.read() << 8) + Wire.read();
}

void i2cSendPosition(int16_t theta, int16_t r) {
  Wire.beginTransmission(I2C_ADDR);

  i2cWriteShort(theta);
  i2cWriteShort(r);

  int err = Wire.endTransmission();
  if (err) {
    Serial.print("send err: ");
    Serial.println(err);
    return;
  }
}

// Automatically called by Wire library when a i2c packet is recieved
void receiveEvent(int howMany) {
  if (howMany != 4) {
    // Must be a problem
    return;
  }

  spinThetaGoal = i2cReadShort();
  linearStepperGoal = i2cReadShort();
}

void i2cMasterTest() {
  // Sends a trio of example commands to tet the protocol and the parsing
  Serial.println("i2cMasterTest...");
  delay(3000);
  Serial.println("begin");
  int delayAmt = 500;
  Serial.println("sendPositionToI2c(0,0);...");
  i2cSendPosition(0, 0);
  delay(delayAmt);
  Serial.println("sendPositionToI2c(16,1400);");
  i2cSendPosition(16, 1400);
  delay(delayAmt);
  Serial.println("sendPositionToI2c(8,-1400);");
  i2cSendPosition(8, -1400);
  delay(delayAmt);
  Serial.println("Complete.");
}
