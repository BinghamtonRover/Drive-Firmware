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

// TODO: Move this to Protobuf
typedef enum MotorErrorCode {
  NO_FAULT = 0,
  MOTOR_OVER_TEMP = 1,
  OVER_CURRENT = 2,
  OVER_VOLTAGE = 3,
  UNDER_VOLTAGE = 4,
  ENCODER_FAULT = 5,
  MOSFET_OVER_TEMP = 6,
  UNKNOWN_ERROR = 7
} MotorErrorCode;

typedef struct MotorData {
  float speed;
  float current;
  int8_t temperature;
  MotorErrorCode error;
} MotorData;


void Motors::handleMotorOutput(uint32_t id, const uint8_t* Data, int length) {
  // The motor sends an 8-byte payload:
  MotorData data;

  // - Position as a signed, 16-bit integer on bytes 0 and 1, unused
  // - Speed as a signed, 16-bit integer on bytes 2 and 3, multiplied by 10
  int16_t speed_int = (Data[2] << 8) | Data[3];
  data.speed = speed_int * 10.0;

  // - Current as a signed, 16-bit integer on bytes 4 and 5, multipled by 0.01
  int16_t current_int = (Data[4] << 8) | Data[5];
  data.current = current_int * 0.01;

  // - Temperature as a signed, 8-byte integer on byte 6
  data.temperature = Data[6];

    // Extract temperature and error code (1 byte each)
    data.motorTemp = Data[6];     // Motor Temperature

    // Extract motor error code
    uint8_t error_code = Data[7];

    data.error = error_code <= UNKNOWN_ERROR
      ? (MotorErrorCode) error_code : UNKNOWN_ERROR;
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
