#include "src/button/button.h"
#include "src/motors/motors.h"
#include "src/led_strip/led_strip.h"
#include "src/cameras/cameras.h"
#include "src/voltage/voltage.h"

// LedButton(led pin, button pin)
LedButton green(12, 13);
LedButton yellow(14, 15);
Motors motors;
Cameras cameras;
LedStrip led_strip;
VoltageSensor voltageSensor(A17);
