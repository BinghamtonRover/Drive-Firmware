#include "current.h"

// uses 12-bit analogRead resolution instead of 10-bit, means other analogread's should set it back to 10 if necessary

const float SENS_SCALE = 100.0;  // Scale for current sensor (each one is different)

// note: should use floats to prevent integer division

CurrentSensor::CurrentSensor(int pin) : pin(pin)
{
}

void CurrentSensor::setup() // Sets up the Teensy to read from the voltage sensor
{
    pinMode(pin, INPUT);
}

float CurrentSensor::read()
{
    analogReadResolution(12);

    float raw = analogRead(pin);

    float current = raw / SENS_SCALE;

    return current;

}

void CurrentSensor::update()
{
    data.currentData = read();
    Serial.println(data.currentData); // test line
}