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

void Motors::sendMotorCommands(BurtCan<Can1> &can) {
  can.sendRaw(leftMotor1, leftBuffer, 4);
  can.sendRaw(leftMotor2, leftBuffer, 4);
  can.sendRaw(leftMotor3, leftBuffer, 4);
  can.sendRaw(rightMotor1, rightBuffer, 4);
  can.sendRaw(rightMotor2, rightBuffer, 4);
  can.sendRaw(rightMotor3, rightBuffer, 4);
}

void Motors::handleMotorOutput(const uint8_t* Data, int length) {
    Serial.println("Got can Message");
    typedef struct MotorData {
        float motorSpeed;
        float motorCurrent;
        int8_t motorTemp;
        int8_t motorError;
    } MotorData;

    // Define enum for error codes
    enum MotorErrorCode {
        NO_FAULT = 0,
        MOTOR_OVER_TEMP = 1,
        OVER_CURRENT = 2,
        OVER_VOLTAGE = 3,
        UNDER_VOLTAGE = 4,
        ENCODER_FAULT = 5,
        MOSFET_OVER_TEMP = 6,
        UNKNOWN_ERROR = 7
    };

    MotorData data;

    // Extract motor speed (2 bytes: Data[2] and Data[3])
    int16_t speed_int = (Data[2] << 8) | Data[3];

    // Extract motor current (2 bytes: Data[4] and Data[5])
    int16_t current_int = (Data[4] << 8) | Data[5];

    // Convert to floats with scaling factors and assign to pointers
    data.motorSpeed = (float)(speed_int * 10.0); // Motor Speed (scaled by 10.0)
    data.motorCurrent = (float)(current_int * 0.01); // Motor Current (scaled by 0.01)

    // Extract temperature and error code (1 byte each)
    data.motorTemp = Data[6];     // Motor Temperature

    // Extract motor error code
    uint8_t error_code = Data[7];

    // C-style cast to MotorErrorCode enum
    if (error_code <= UNKNOWN_ERROR) { 
        data.motorError = error_code;
    }
    else {
        data.motorError = UNKNOWN_ERROR;  // Handl unknown error codes, in case the motor sends broken data
    }
    Serial.println(data.motorSpeed);

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
