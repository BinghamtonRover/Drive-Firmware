#include "src/utils/BURT_utils.h"
#include "src/BURT_vesc.h"
#include "src/drive.pb.h"

#define DRIVE_COMMAND_ID 0x53
#define DRIVE_DATA_ID    0x14

VescDriver left1(1);
VescDriver left2(3);
VescDriver left3(4);
VescDriver right1(5);
VescDriver right2(6);
VescDriver right3(7);
float leftVelocity, rightVelocity, throttle;
int canSendInterval = 500;
unsigned long nextSendTime;

void handleDriveCommand(const uint8_t* data, int length) {
	auto command = BurtProto::decode<DriveCommand>(data, length, DriveCommand_fields);
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

	left1.setVelocity(leftVelocity, throttle);
	left2.setVelocity(leftVelocity, throttle);
	left3.setVelocity(leftVelocity, throttle);
	right1.setVelocity(rightVelocity, throttle);
	right2.setVelocity(rightVelocity, throttle);
	right3.setVelocity(rightVelocity, throttle);
}

BurtSerial serial(handleDriveCommand, Device::Device_DRIVE);
BurtCan can(DRIVE_COMMAND_ID, handleDriveCommand);

void sendData() {
  if (millis() < nextSendTime) return;
	DriveData data = DriveData_init_zero;
	data.left = leftVelocity;
  data.set_left = true;
	can.send(DRIVE_DATA_ID, &data, DriveData_fields);

	data = DriveData_init_zero;
	data.right = rightVelocity;
  data.set_right = true;
	can.send(DRIVE_DATA_ID, &data, DriveData_fields);

	data = DriveData_init_zero;
	data.throttle = throttle;
  data.set_throttle = true;
	can.send(DRIVE_DATA_ID, &data, DriveData_fields);

  nextSendTime = millis() + canSendInterval;
}

void setup() {
	Serial.begin(9600);
  Serial.println("Initializing Drive subsystem");

  Serial.println("Initializing communications...");
	can.setup();
  nextSendTime = millis() + canSendInterval;

  Serial.println("Initializing VESC motor drivers...");
	left1.setup();
	left2.setup();
	left3.setup();
	right1.setup();
	right2.setup();
	right3.setup();

  Serial.println("Drive subsystem initialized");
}

void loop() {
	can.update();
	serial.update();
	sendData();
  delay(1);
}
