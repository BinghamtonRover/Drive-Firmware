#include <Arduino.h>
#include "temperature.h"

TemperatureSensor::TemperatureSensor(int pin) : 
  pin(pin)
  { }

void TemperatureSensor::setup() {
  pinMode(pin, INPUT);
}

void TemperatureSensor::update() {
  float voltage = analogRead(pin) * 3.3 / 1023.0;
  float temperature = ((5.506 - sqrt(sq(-5.506) + (4*0.00176 * (870.6 - voltage)))) / (2 * (-0.00176))) + 30;
  data.battery_temperature = temperature;
}
