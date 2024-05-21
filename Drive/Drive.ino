#include <Servo.h>

#include "src/utils/BURT_utils.h"
#include "src/drive.pb.h"

#include "src/leds/leds.h"

#define DRIVE_COMMAND_ID   0x53
#define DRIVE_DATA_ID      0x14

#define FRONT_SWIVEL 4
#define FRONT_TILT 5
#define BACK_SWIVEL 33
#define BACK_TILT 36

#define VOLTAGE_SENSOR A17

#define DATA_SEND_INTERVAL 250  // ms
#define MOTOR_UPDATE_INTERVAL 0  // ms

#define MAX_DRIVE_SPEED 40000  // E-RPM

// ----- Motor CAN IDs ----
#define LEFT_MOTOR_1_ID  0x302 //current config of motors
#define LEFT_MOTOR_2_ID  0x307 // Motor 1 is closest to diff bar
#define LEFT_MOTOR_3_ID  0x301
#define RIGHT_MOTOR_1_ID 0x303
#define RIGHT_MOTOR_2_ID 0x308
#define RIGHT_MOTOR_3_ID 0x305

const Version version = {major: 1, minor: 0};

Servo frontSwivel, frontTilt;
Servo backSwivel, backTilt;



float leftVelocity, rightVelocity, throttle;

void handleDriveCommand(const uint8_t* data, int length);
void handleMotorOutput(const uint8_t* data, int length) { /* TODO: Implement this */}
void shutdown() { throttle = 0; updateSpeeds(); }
void sendData();
void updateMotors();

BurtSerial serial(Device::Device_DRIVE, handleDriveCommand, shutdown);
BurtCan<Can3> roverCan(DRIVE_COMMAND_ID, Device::Device_DRIVE, handleDriveCommand, shutdown);
BurtCan<Can1> motorCan(0, Device::Device_DRIVE, handleMotorOutput, shutdown, true);
BurtTimer dataTimer(DATA_SEND_INTERVAL, sendData);
BurtTimer motorTimer(MOTOR_UPDATE_INTERVAL, updateMotors);

uint8_t leftData[4] = {0, 0, 0, 0};
uint8_t rightData[4] = {0, 0, 0, 0};

LedStrip leds;

float read_voltage() {
	float voltage = analogRead(VOLTAGE_SENSOR);
	return voltage / 1023.0 * 3.3 * 11.0;  // 1023 represents 3.3V through an 11 ohm resistor
}

void setup() {
	Serial.begin(9600);
  Serial.println("Initializing Drive subsystem");

  Serial.println("Initializing communications...");
	roverCan.setup();
	motorCan.setup();
	serial.setup();
	dataTimer.setup();
	motorTimer.setup();

	Serial.println("Initializing servos...");
	frontSwivel.attach(FRONT_SWIVEL);
	frontTilt.attach(FRONT_TILT);
	backSwivel.attach(BACK_SWIVEL);
	backTilt.attach(BACK_TILT);

	leds.setup();
	leds.blue();

  Serial.println("Drive subsystem initialized");
}

uint8_t version_buffer[8] = {0x72, 0x04, 0x08, 0x01, 0x10, 0x00};

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
  serial.send(DriveData_fields, &data, 8);

  // data = DriveData_init_zero;
	// data.version = version;
	// roverCan.send(DRIVE_DATA_ID, &data, DriveData_fields);
  // serial.send(DriveData_fields, &data, 64);
  Serial.write(version_buffer, 6);
  roverCan.sendRaw(DRIVE_DATA_ID, version_buffer, 6);

	data = DriveData_init_zero;
	data.right = rightVelocity;
  data.set_right = true;
	roverCan.send(DRIVE_DATA_ID, &data, DriveData_fields);
  serial.send(DriveData_fields, &data, 8);

	data = DriveData_init_zero;
	data.throttle = throttle;
  data.set_throttle = true;
	roverCan.send(DRIVE_DATA_ID, &data, DriveData_fields);
  serial.send(DriveData_fields, &data, 8);

	data = DriveData_init_zero;
	data.battery_voltage = read_voltage();
	roverCan.send(DRIVE_DATA_ID, &data, DriveData_fields);
  serial.send(DriveData_fields, &data, 8);

  data = DriveData_init_zero;
	data.front_swivel = 15;
	roverCan.send(DRIVE_DATA_ID, &data, DriveData_fields);
  serial.send(DriveData_fields, &data, 8);
}

void updateSpeeds() {
	updateHexSpeed(leftVelocity, leftData);
	updateHexSpeed(rightVelocity, rightData);
}

void handleDriveCommand(const uint8_t* data, int length) {
	auto command = BurtProto::decode<DriveCommand>(data, length, DriveCommand_fields);
	// Update motors
	if (command.set_throttle) {
    throttle = command.throttle;
  }
	if (command.set_left) {
		leftVelocity = command.left;
	}
	if (command.set_right) {
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
