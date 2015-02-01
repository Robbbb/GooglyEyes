// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.

#include <Wire.h>

void setup() {
  Serial.begin(9600);
  Wire.begin();  // join i2c bus (address optional for master)
  Serial.println("Master writer, bare minumum");  // print the integer
}

void i2cWriteShort(short data) {
  int8_t lo = (data >> 0) & 0xFF;
  int8_t hi = (data >> 8) & 0xFF;

  Wire.write(hi);
  Wire.write(lo);
}

int i = -128;

void loop() {
  delay(500);

  Wire.beginTransmission(4);  // transmit to device #4

  i2cWriteShort(i);
  i++;

  i2cWriteShort(i);
  i++;

  int err = Wire.endTransmission();

  if (err) {
    Serial.print("error ");
    Serial.println(err);
    return;
  }

  Serial.println("sent");
}
