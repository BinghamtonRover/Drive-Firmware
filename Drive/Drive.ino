#include <Servo.h>

#include "src/utils/BURT_utils.h"
#include "src/drive.pb.h"

#include "src/led_strip/led_strip.h"

#define DRIVE_COMMAND_ID   0x53
#define DRIVE_DATA_ID      0x14

#define FRONT_SWIVEL 4
#define FRONT_TILT 5
#define BACK_SWIVEL 33
#define BACK_TILT 36

#define VOLTAGE_SENSOR A17

#define DATA_SEND_INTERVAL 250  // ms
#define MOTOR_UPDATE_INTERVAL 0  // ms

const Version version = {major: 1, minor: 0};

Servo frontSwivel, frontTilt;
Servo backSwivel, backTilt;

float leftVelocity, rightVelocity, throttle;

void handleDriveCommand(const uint8_t* data, int length);
void handleMotorOutput(const uint8_t* data, int length) { /* TODO: Implement this */}
void shutdown() { throttle = 0; }
void sendData();
void updateMotors() { }

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

  data = DriveData_init_zero;
	data.version = version;
	data.has_version = true;
	roverCan.send(DRIVE_DATA_ID, &data, DriveData_fields);
  serial.send(DriveData_fields, &data, 8);

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

void handleDriveCommand(const uint8_t* data, int length) {
	auto command = BurtProto::decode<DriveCommand>(data, length, DriveCommand_fields);
	
}
