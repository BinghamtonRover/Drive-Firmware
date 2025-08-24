#include "src/button/button.h"
#include "src/motors/motors.h"
#include "src/led_strip/led_strip.h"
#include "src/cameras/cameras.h"
#include "src/voltage/voltage.h"
#include "src/temperature/temperature.h"

Buttons buttons;     // see src/button/button.h for pinouts
Motors motors;       // see src/motors/motors.h for CAN IDs
Cameras cameras;     // see src/cameras/cameras for pinouts
LedStrip led_strip;  // see src/led_strip/led_strip for pinouts

VoltageSensor voltageSensor(41);
TemperatureSensor temperatureSensor(21);
