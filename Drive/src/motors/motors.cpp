#include "motors.h"

void Motors::updateBuffer(float speed, uint8_t* buffer) {
	int adjusted = maxRpm * throttle * speed;
	if (abs(adjusted) < 5) {
		adjusted = 0;
	}
	buffer[0] = (adjusted & 0xFF000000) >> 24;
	buffer[1] = (adjusted & 0x00FF0000) >> 16;
	buffer[2] = (adjusted & 0x0000FF00) >> 8;
	buffer[3] = (adjusted & 0x000000FF);
}

void Motors::updateBuffers() {
	updateBuffer(left, leftBuffer);
	updateBuffer(right, rightBuffer);
}

void Motors::sendMotorCommands(BurtCan<Can1>& can) {
	can.sendRaw((0x3 << 8) | front_left_motor_id, leftBuffer, 4);
	can.sendRaw((0x3 << 8) | middle_left_motor_id, leftBuffer, 4);
	can.sendRaw((0x3 << 8) | back_left_motor_id, leftBuffer, 4);
	can.sendRaw((0x3 << 8) | front_right_motor_id, rightBuffer, 4);
	can.sendRaw((0x3 << 8) | middle_right_motor_id, rightBuffer, 4);
	can.sendRaw((0x3 << 8) | back_right_motor_id, rightBuffer, 4);
}

void Motors::handleMotorOutput(uint32_t id, const uint8_t* rawData, int length) {
	// The motor sends an 8-byte payload:
	DriveMotorData motorData = DriveMotorData_init_zero;
	// - Position as a signed, 16-bit integer on bytes 0 and 1, unused
	// - Speed as a signed, 16-bit integer on bytes 2 and 3, multiplied by 10
	int16_t speedInt = static_cast<int16_t>((rawData[2] << 8) | rawData[3]);
	motorData.speed = speedInt * 10.0f;

	// - Current as a signed, 16-bit integer on bytes 4 and 5, multipled by 0.01
	int16_t currentInt = static_cast<int16_t>(rawData[4] << 8) | rawData[5];
	motorData.current = currentInt * 0.01f;

	// - Temperature as a signed, 8-bit integer on byte 6
	motorData.temperature = static_cast<int16_t>(rawData[6]);

	// Extract motor error code
	uint8_t errorCode = rawData[7];

	// Max Error code is 7
	motorData.error = static_cast<MotorErrorCode>(errorCode <= 7 ? errorCode : 7);

	// Set motorData to current field
	switch (id & 0xFF) {
	case front_left_motor_id:
		data.back_left_motor = motorData;
		data.has_back_left_motor = true;
		break;
	case middle_left_motor_id:
		data.middle_left_motor = motorData;
		data.has_middle_left_motor = true;
		break;
	case back_left_motor_id:
		data.front_left_motor = motorData;
		data.has_front_left_motor = true;
		break;
	case front_right_motor_id:
		data.back_right_motor = motorData;
		data.has_back_right_motor = true;
		break;
	case middle_right_motor_id:
		data.middle_right_motor = motorData;
		data.has_middle_right_motor = true;
		break;
	case back_right_motor_id:
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
