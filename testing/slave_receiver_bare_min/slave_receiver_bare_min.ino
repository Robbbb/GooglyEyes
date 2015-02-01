// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.

#include <Wire.h>

void setup() {
  Wire.begin(4);                 // join i2c bus with address #4
  Serial.begin(9600);            // start serial for output
  Wire.onReceive(receiveEvent);  // register event
  // Serial.println("Wire Slave Receiver, bare minumum");  // print the integer
}

int a, b;

void loop() {
  delay(100);

  char output[100];
  snprintf(output, 100, "a=%d b=%d", a, b);
  Serial.println(output);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  if (howMany != 4) {
    return;
  }

  a = (Wire.read() << 8) + Wire.read();
  b = (Wire.read() << 8) + Wire.read();
}
