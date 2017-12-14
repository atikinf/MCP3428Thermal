#include <MCP3428Thermal.h>
#include <Wire.h>

MCP3428Thermal ADCTest;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println(ADCTest.testConnection() ? "Connected" : "Not Connected");
}

void loop() {
  int16_t* dataPtr;
  if (ADCTest.readRegister(dataPtr, 1) != -1) {
    Serial.print(*dataPtr);
  }
}
