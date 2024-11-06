#include "src/button/button.h"
#include "src/motors/motors.h"
#include "src/led_strip/led_strip.h"
#include "src/cameras/cameras.h"
#include "src/voltage/voltage.h"
#include "src/temperature/temperature.h"

Buttons buttons;
Motors motors;
Cameras cameras;
LedStrip led_strip;
VoltageSensor voltageSensor(41);
TemperatureSensor temperatureSensor(21);
