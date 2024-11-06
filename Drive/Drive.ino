#include <Servo.h>

#include "pinouts.h"
#include "src/utils/BURT_utils.h"
#include "src/drive.pb.h"

#define DRIVE_COMMAND_ID   0x53
#define DRIVE_DATA_ID      0x14

#define DATA_SEND_INTERVAL 250  // ms
#define MOTOR_UPDATE_INTERVAL 0  // ms

const Version version = {major: 1, minor: 1};

void handleCommand(const uint8_t* data, int length);
void handleMotorOutput(const uint8_t* data, int length) { motors.handleMotorOutput(data, length); }

BurtSerial serial(Device::Device_DRIVE, handleCommand, DriveData_fields, DriveData_size);
BurtCan<Can3> roverCan(DRIVE_COMMAND_ID, handleCommand);
BurtCan<Can1> motorCan(0, handleMotorOutput, true);

void sendData();
void updateMotors() { motors.sendMotorCommands(motorCan); }
void updateLedStrip() { led_strip.update(); }

BurtTimer dataTimer(DATA_SEND_INTERVAL, sendData);
BurtTimer motorTimer(MOTOR_UPDATE_INTERVAL, updateMotors);
BurtTimer blinkTimer(blinkInterval, updateLedStrip);

void setup() {
	Serial.begin(9600);
  Serial.println("Initializing Drive subsystem");

  Serial.println("Initializing software...");
	roverCan.setup();
	motorCan.setup();
	serial.setup();
	dataTimer.setup();
	motorTimer.setup();
	blinkTimer.setup();

	Serial.println("Initializing hardware...");
	motors.setup();
	buttons.setup();
	cameras.setup();
	led_strip.setup();
	voltageSensor.setup();
	temperatureSensor.setup();

  Serial.println("Drive subsystem initialized");
}

void loop() {
	serial.update();
	roverCan.update();
	motorCan.update();
	dataTimer.update();
	motorTimer.update();
	blinkTimer.update();
	buttons.update();
	voltageSensor.update();
	temperatureSensor.update();
}

void sendData() {
  DriveData data = DriveData_init_zero;
  data.version = version;
	data.has_version = true;
  serial.send(&data);
	serial.send(&buttons.data);
	serial.send(&motors.data);
	serial.send(&cameras.data);
	serial.send(&led_strip.data);
	serial.send(&voltageSensor.data);
	// serial.send(&temperatureSensor.data);
}

void handleCommand(const uint8_t* data, int length) {
	auto command = BurtProto::decode<DriveCommand>(data, length, DriveCommand_fields);
	buttons.handleCommand(command);
	motors.handleCommand(command);
	cameras.handleCommand(command);
	led_strip.handleCommand(command);
}
