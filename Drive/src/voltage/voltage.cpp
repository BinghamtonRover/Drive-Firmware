#include <Arduino.h>
#include "voltage.h"

VoltageSensor::VoltageSensor(int pin) : 
  pin(pin)
  { }

void VoltageSensor::setup() {
  pinMode(pin, INPUT);
}

float VoltageSensor::read() {
  float voltage = analogRead(pin);
  // 1023 represents 3.3V through an 11 ohm resistor
	return voltage / 1023.0 * 3.3 * 11.0; 
}

void VoltageSensor::update() {
  data.battery_voltage = read();
}
