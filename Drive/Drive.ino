#include <Servo.h>

#include "pinouts.h"
#include "src/utils/BURT_utils.h"
#include "src/drive.pb.h"

#define DRIVE_COMMAND_ID   0x53
#define DRIVE_DATA_ID      0x14

#define DATA_SEND_INTERVAL 250  // ms
#define MOTOR_UPDATE_INTERVAL 0  // ms

const Version version = {major: 1, minor: 0};

void handleCommand(const uint8_t* data, int length);
void handleMotorOutput(const uint8_t* data, int length) { /* TODO: Implement this */}
void shutdown() { motors.stop(); }
void sendData();
void updateMotors() { }

BurtSerial serial(Device::Device_DRIVE, handleCommand, shutdown);
BurtCan<Can3> roverCan(DRIVE_COMMAND_ID, Device::Device_DRIVE, handleCommand, shutdown);
BurtCan<Can1> motorCan(0, Device::Device_DRIVE, handleMotorOutput, shutdown, true);
BurtTimer dataTimer(DATA_SEND_INTERVAL, sendData);
BurtTimer motorTimer(MOTOR_UPDATE_INTERVAL, updateMotors);

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
	cameras.setup();

	led_strip.setup();
	led_strip.blue();

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
	data.left = motors.left;
  data.set_left = true;
	roverCan.send(DRIVE_DATA_ID, &data, DriveData_fields);
  serial.send(DriveData_fields, &data, 8);
  
  data = DriveData_init_zero;
	data.right = motors.right;
  data.set_right = true;
	roverCan.send(DRIVE_DATA_ID, &data, DriveData_fields);
  serial.send(DriveData_fields, &data, 8);

	data = DriveData_init_zero;
	data.throttle = motors.throttle;
  data.set_throttle = true;
	roverCan.send(DRIVE_DATA_ID, &data, DriveData_fields);
  serial.send(DriveData_fields, &data, 8);

  data = DriveData_init_zero;
	data.version = version;
	data.has_version = true;
	roverCan.send(DRIVE_DATA_ID, &data, DriveData_fields);
  serial.send(DriveData_fields, &data, 8);

	data = DriveData_init_zero;
	data.battery_voltage = voltageSensor.read();
	roverCan.send(DRIVE_DATA_ID, &data, DriveData_fields);
  serial.send(DriveData_fields, &data, 8);

  data = DriveData_init_zero;
	data.front_swivel = 15;
	roverCan.send(DRIVE_DATA_ID, &data, DriveData_fields);
  serial.send(DriveData_fields, &data, 8);
}

void handleCommand(const uint8_t* data, int length) {
	auto command = BurtProto::decode<DriveCommand>(data, length, DriveCommand_fields);

}
