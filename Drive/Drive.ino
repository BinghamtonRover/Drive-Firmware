#include <BURT_can.h>
#include <BURT_proto.h>
#include <BURT_serial.h>

#include "src/BURT_vesc.h"
#include "src/drive.pb.h"

#define DRIVE_COMMAND_ID 0x53
#define DRIVE_DATA_ID    0x14

#define LEFT_PIN  2
#define RIGHT_PIN 4

BurtSerial serial(handleDriveCommand);
VescDriver left(LEFT_PIN), right(RIGHT_PIN);
float leftVelocity, rightVelocity, throttle;

void handleDriveCommand(const uint8_t* data, int length) {
	auto command = BurtProto::decode<DriveCommand>(data, length, DriveCommand_fields);
	if (command.set_throttle) throttle = command.throttle;
	if (command.set_left) leftVelocity = command.left;
	if (command.set_right) rightVelocity = command.right;

	left.setVelocity(leftVelocity, throttle);
	right.setVelocity(rightVelocity, throttle);
	// delay(200);  // don't thrash the wheels
}

void sendData() {
	DriveData data1, data2, data3;
	data1.left = leftVelocity;
	data2.right = rightVelocity;
	data3.throttle = throttle;
	BurtCan::send(DRIVE_DATA_ID, DriveData_fields, &data1);
	BurtCan::send(DRIVE_DATA_ID, DriveData_fields, &data2);
	BurtCan::send(DRIVE_DATA_ID, DriveData_fields, &data3);
  // Serial.println("Sent data");
}

void setup() {
	Serial.begin(9600);
	BurtCan::setup();
	BurtCan::registerHandler(DRIVE_COMMAND_ID, handleDriveCommand);
	left.setup();
	right.setup();
}

void loop() {
	BurtCan::update();
	serial.parseSerial();
	// sendData();
	delay(10);
}
