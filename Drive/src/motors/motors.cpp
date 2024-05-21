#include "motors.h"

const int maxRpm = 40'000;

const int leftMotor1 = 0x302;
const int leftMotor2 = 0x307;
const int leftMotor3 = 0x301;
const int rightMotor1 = 0x303;
const int rightMotor2 = 0x308;
const int rightMotor3 = 0x305;

void Motors::updateBuffer(float speed, uint8_t* buffer) {
  int adjusted = maxRpm * throttle * speed;
	if (abs(adjusted) < 5) adjusted = 0;
  buffer[0] = (adjusted & 0xFF000000) >> 24;
  buffer[1] = (adjusted & 0x00FF0000) >> 16;
  buffer[2] = (adjusted & 0x0000FF00) >> 8;
  buffer[3] = (adjusted & 0x000000FF);
}

void Motors::updateBuffers() {
	updateBuffer(left, leftBuffer);
	updateBuffer(right, rightBuffer);
}


void Motors::sendMotorCommands(BurtCan<Can1> can) {
  can.sendRaw(leftMotor1, leftBuffer, 4);
  can.sendRaw(leftMotor2, leftBuffer, 4);
  can.sendRaw(leftMotor3, leftBuffer, 4);
  can.sendRaw(rightMotor1, rightBuffer, 4);
  can.sendRaw(rightMotor2, rightBuffer, 4);
  can.sendRaw(rightMotor3, rightBuffer, 4);
}

void Motors::handleCommand(DriveCommand command) {
	if (command.set_throttle) {
    throttle = command.throttle;
  }
	if (command.set_left) {
		left = command.left;
	}
	if (command.set_right) {
		right = command.right;
	}
}

void Motors::stop() {
  throttle = 0;
  updateBuffers();
}

void Motors::setSpeeds(float newLeft, float newRight) {
  left = newLeft;
  right = newRight;
  updateBuffers();
}

void Motors::setThrottle(float newThrottle) {
  throttle = newThrottle;
  updateBuffers();
}
