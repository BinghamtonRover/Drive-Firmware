#include "BURT_vesc.h"

VescDriver::VescDriver(int pin) : pin(pin) {}

void VescDriver::setup() {
	pinMode(pin, OUTPUT);
	driver.attach(pin, PULSE_WIDTH_MAX_NEG, PULSE_WIDTH_MAX_POS);
}

void VescDriver::setVelocity(float velocity, float throttle) {
	uint32_t pulse_width;
	if (velocity < -1 || velocity > 1) {
		Serial.println("ERROR: Velocity must be between -1 and 1");
		setVelocity(0, 0);
		while (true);  // refuse to continue
	}
	velocity = velocity * MAX_SPEED * throttle;
	if (velocity == 0) {
	  pulse_width = PULSE_WIDTH_ZERO;
	} else if (velocity > 0) {
		pulse_width = (uint32_t)(PULSE_WIDTH_MIN_POS + velocity * (PULSE_WIDTH_MAX_POS - PULSE_WIDTH_MIN_POS) / (MAX_SPEED));
	} else {
		pulse_width = (uint32_t)(PULSE_WIDTH_MIN_NEG + velocity * (PULSE_WIDTH_MAX_NEG - PULSE_WIDTH_MIN_NEG) / (-MAX_SPEED));
	}
	
	driver.writeMicroseconds(pulse_width);
}
