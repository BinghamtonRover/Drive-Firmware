#pragma once

#include <Arduino.h>
#include <Servo.h>

#define PULSE_WIDTH_MAX_POS   2000  // pulse width for maximum forward velocity (us)
#define PULSE_WIDTH_MIN_POS   1650  // pulse width for minimum forward velocity (us)
#define PULSE_WIDTH_ZERO      1500  // pulse width for zero velocity (us)
#define PULSE_WIDTH_MIN_NEG   1350  // pulse width for minimum backwards velocity (us)
#define PULSE_WIDTH_MAX_NEG   1000  // pulse width for maximum backwards velocity (us)

#define MAX_SPEED    10.0
#define VEL_ZERO      0.0

class VescDriver {
	public:
		VescDriver(int pin);    
		void setup();
		/* -1.0 <= velocity <= 1.0, 0 <= throttle <= 1.0 */
		void setVelocity(float velocity, float throttle);

	private:
		Servo driver;
		int pin;
};
