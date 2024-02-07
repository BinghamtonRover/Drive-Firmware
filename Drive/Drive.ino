#include <Servo.h>

#include "src/utils/BURT_utils.h"
#include "src/drive.pb.h"

#define DRIVE_COMMAND_ID 0x53
#define DRIVE_DATA_ID    0x14

#define FRONT_SWIVEL 11
#define FRONT_TILT 29
#define BACK_SWIVEL 12
#define BACK_TILT 28

#define DATA_SEND_INTERVAL 50  // ms
#define MOTOR_UPDATE_INTERVAL 0  // ms

#define MAX_DRIVE_SPEED 20000  // E-RPM

// ----- Motor CAN IDs ----
#define LEFT_MOTOR_1_ID  0x301
#define LEFT_MOTOR_2_ID  0x303
#define LEFT_MOTOR_3_ID  0x308
#define RIGHT_MOTOR_1_ID 0x302
#define RIGHT_MOTOR_2_ID 0x307
#define RIGHT_MOTOR_3_ID 0x305

Servo frontSwivel, frontTilt;
Servo backSwivel, backTilt;

float leftVelocity, rightVelocity, throttle;

void handleDriveCommand(const uint8_t* data, int length);
void handleMotorOutput(const uint8_t* data, int length) { /* TODO: Implement this */}
void shutdown() { throttle = 0; updateSpeeds(); }
void sendData();
void updateMotors();

BurtSerial serial(Device::Device_CONTROL, handleDriveCommand, shutdown);
BurtCan<Can3> roverCan(DRIVE_COMMAND_ID, Device::Device_CONTROL, handleDriveCommand, shutdown);
BurtCan<Can1> motorCan(0, Device::Device_CONTROL, handleMotorOutput, shutdown, true);
BurtTimer dataTimer(DATA_SEND_INTERVAL, sendData);
BurtTimer motorTimer(MOTOR_UPDATE_INTERVAL, updateMotors);

uint8_t leftData[4] = {0, 0, 0, 0};
uint8_t rightData[4] = {0, 0, 0, 0};

void setup() {
	Serial.begin(9600);
  Serial.println("Initializing Drive subsystem");

  Serial.println("Initializing communications...");
	roverCan.setup();
	motorCan.setup();
	serial.setup();
	roverCan.showDebugInfo();
	dataTimer.setup();
	motorTimer.setup();

	Serial.println("Initializing servos...");
	frontSwivel.attach(FRONT_SWIVEL);
	frontTilt.attach(FRONT_TILT);
	backSwivel.attach(BACK_SWIVEL);
	backTilt.attach(BACK_TILT);

  Serial.println("Drive subsystem initialized");
}

void loop() {
	serial.update();
	roverCan.update();
	motorCan.update();
	dataTimer.update();
	motorTimer.update();
}

void sendData() {
	DriveData data = DriveData_init_zero;
	data.left = leftVelocity;
  data.set_left = true;
	roverCan.send(DRIVE_DATA_ID, &data, DriveData_fields);

	data = DriveData_init_zero;
	data.right = rightVelocity;
  data.set_right = true;
	roverCan.send(DRIVE_DATA_ID, &data, DriveData_fields);

	data = DriveData_init_zero;
	data.throttle = throttle;
  data.set_throttle = true;
	roverCan.send(DRIVE_DATA_ID, &data, DriveData_fields);
}

void updateSpeeds() {
	updateHexSpeed(leftVelocity, leftData);
	updateHexSpeed(rightVelocity, rightData);
}

void handleDriveCommand(const uint8_t* data, int length) {
	auto command = BurtProto::decode<DriveCommand>(data, length, DriveCommand_fields);
	// Update motors
	if (command.set_throttle) {
    if (command.throttle != 0) { Serial.print("Throttle: "); Serial.println(command.throttle); }
    throttle = command.throttle;
  }
	if (command.set_left) {
    if (command.left != 0) { Serial.print("Left: "); Serial.println(command.left); }
		leftVelocity = command.left;
	}
	if (command.set_right) {
    if (command.right != 0) { Serial.print("Right: "); Serial.println(command.right); }
		rightVelocity = command.right;
	}
	updateSpeeds();

	// Update servos
	if (command.front_swivel != 0) frontSwivel.write(command.front_swivel);
	if (command.front_tilt != 0) frontTilt.write(command.front_tilt);
	if (command.rear_swivel != 0) backSwivel.write(command.rear_swivel);
	if (command.rear_tilt != 0) backTilt.write(command.rear_tilt);
}

void updateHexSpeed(float speed_percent, uint8_t* buffer) {
	int speed = MAX_DRIVE_SPEED * throttle * speed_percent;
	if (abs(speed) < 5) speed = 0;
  buffer[0] = (speed & 0xFF000000) >> 24;
  buffer[1] = (speed & 0x00FF0000) >> 16;
  buffer[2] = (speed & 0x0000FF00) >> 8;
  buffer[3] = (speed & 0x000000FF);
}

void updateMotors() {
	motorCan.sendRaw(LEFT_MOTOR_1_ID, leftData, 4);
  motorCan.sendRaw(LEFT_MOTOR_2_ID, leftData, 4);
  motorCan.sendRaw(LEFT_MOTOR_3_ID, leftData, 4);
  motorCan.sendRaw(RIGHT_MOTOR_1_ID, rightData, 4);
  motorCan.sendRaw(RIGHT_MOTOR_2_ID, rightData, 4);
  motorCan.sendRaw(RIGHT_MOTOR_3_ID, rightData, 4);
}
