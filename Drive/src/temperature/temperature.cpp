#include <Arduino.h>
#include "temperature.h"

TemperatureSensor::TemperatureSensor(int pin) :
  pin(pin)
  { }

void TemperatureSensor::setup() {
  pinMode(pin, INPUT);
}

float TemperatureSensor::read() {
  float voltage = analogRead(pin) * 3.3 / 1023.0;
  // Note: this formula matches the quadratic formula
  return ((5.506 - sqrt(sq(-5.506) + (4*0.00176 * (870.6 - voltage)))) / (2 * (-0.00176))) + 30;
}

void TemperatureSensor::update() {
  data.battery_temperature = read();
}
