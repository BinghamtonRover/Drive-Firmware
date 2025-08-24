#include "motors.h"

const int maxRpm = 40'000;

const int leftMotor1 = 0x302;
const int leftMotor2 = 0x305;
const int leftMotor3 = 0x30c;
const int rightMotor1 = 0x30d;
const int rightMotor2 = 0x30a;
const int rightMotor3 = 0x30b;

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

void Motors::sendMotorCommands(BurtCan<Can1> &can) {
  can.sendRaw(leftMotor1, leftBuffer, 4);
  can.sendRaw(leftMotor2, leftBuffer, 4);
  can.sendRaw(leftMotor3, leftBuffer, 4);
  can.sendRaw(rightMotor1, rightBuffer, 4);
  can.sendRaw(rightMotor2, rightBuffer, 4);
  can.sendRaw(rightMotor3, rightBuffer, 4);
}


void Motors::handleMotorOutput(uint32_t id, const uint8_t* Data, int length) {
  // The motor sends an 8-byte payload:
  DriveMotorData motorData;
  // Set Data Container Id to motor Id
  motorData.Id = id & 0xFF;
  // - Position as a signed, 16-bit integer on bytes 0 and 1, unused
  // - Speed as a signed, 16-bit integer on bytes 2 and 3, multiplied by 10
  int16_t speed_int = static_cast<int16_t>((Data[2] << 8) | Data[3]);
  motorData.speed = speed_int * 10.0;

  // - Current as a signed, 16-bit integer on bytes 4 and 5, multipled by 0.01
  int16_t current_int = static_cast<int16_t>(Data[4] << 8) | Data[5];
  motorData.current = current_int * 0.01;

  // - Temperature as a signed, 8-bit integer on byte 6
  motorData.temperature = static_cast<int16_t>(Data[6]);

  // Extract motor error code
  uint8_t error_code = static_cast<int8_t>(Data[7]);

  // Max Error code is 7
  motorData.error = static_cast<MotorErrorCode>(error_code <= 7 ? error_code : 7);

  switch (id) { //Set motorData to current field
  case leftMotor1:
    data.back_left_motor = motorData;
    data.has_back_left_motor = true;
    break;

  case leftMotor2:
    data.middle_left_motor = motorData;
    data.has_middle_left_motor = true;
    break;

  case leftMotor3:
    data.front_left_motor = motorData;
    data.has_front_left_motor = true;
    break;

  case rightMotor1:
    data.back_right_motor = motorData;
    data.has_back_right_motor = true;
    break;

  case rightMotor2:
    data.middle_right_motor = motorData;
    data.has_middle_right_motor = true;
    break;

  case rightMotor3:
    data.front_right_motor = motorData;
    data.has_front_right_motor = true;
    break;

  default:
    // Unknown motor ID
    break;
  }

}

void Motors::setup() {
  data.set_throttle = true;
  data.set_left = true;
  data.set_right = true;
}

void Motors::handleCommand(DriveCommand command) {
	if (command.set_throttle) {
    throttle = command.throttle;
    data.throttle = command.throttle;
  }
	if (command.set_left) {
		left = command.left;
		data.left = command.left;
	}
	if (command.set_right) {
		right = command.right;
		data.right = command.right;
	}
  updateBuffers();
}

void Motors::stop() {
  setThrottle(0);
}

void Motors::setSpeeds(float newLeft, float newRight) {
  left = newLeft;
  right = newRight;
  data.left = left;
  data.right = right;
  updateBuffers();
}

void Motors::setThrottle(float newThrottle) {
  throttle = newThrottle;
  data.throttle = throttle;
  updateBuffers();
}
