#include "src/button/button.h"
#include "src/motors/motors.h"
#include "src/led_strip/led_strip.h"
#include "src/cameras/cameras.h"
#include "src/voltage/voltage.h"

Buttons buttons;
Motors motors;
Cameras cameras;
LedStrip led_strip;
VoltageSensor voltageSensor(A17);
