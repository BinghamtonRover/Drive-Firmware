#include <HCSR04.h>
#include <Servo.h>

#include "src/utils/BURT_utils.h"
#include "src/BURT_vesc.h"
#include "src/drive.pb.h"

#define DRIVE_COMMAND_ID 0x53
#define DRIVE_DATA_ID    0x14

#define FRONT_SWIVEL 11
#define FRONT_TILT 29
#define BACK_SWIVEL 12
#define BACK_TILT 28

#define LEFT_TRIGGER_PIN 19
#define LEFT_ECHO_PIN 20
#define RIGHT_TRIGGER_PIN 34
#define RIGHT_ECHO_PIN 35

#define US_MIN 500
#define US_MAX 2500

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

UltraSonicDistanceSensor leftSensor(LEFT_TRIGGER_PIN, LEFT_ECHO_PIN);
UltraSonicDistanceSensor rightSensor(RIGHT_TRIGGER_PIN, RIGHT_ECHO_PIN);

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

	if (command.front_swivel != 0) moveServo(servo1, command.front_swivel);
	if (command.front_tilt != 0) moveServo(servo2, command.front_tilt);
	if (command.rear_swivel != 0) moveServo(servo3, command.rear_swivel);
	if (command.rear_tilt != 0) moveServo(servo4, command.rear_tilt);
}

void moveServo(Servo& servo, int angle) {
  Serial.print("Moving ");
  Serial.print(angle);
  Serial.println("degrees");
	servo.writeMicroseconds(deg_to_us(angle));
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

	data = DriveData_init_zero;
	data.leftSensorValue = leftSensor.measureDistanceCm();
	can.send(DRIVE_DATA_ID, &data, DriveData_fields);

	data = DriveData_init_zero;
	data.rightSensorValue = rightSensor.measureDistanceCm();
	can.send(DRIVE_DATA_ID, &data, DriveData_fields);

  nextSendTime = millis() + canSendInterval;
}

int deg_to_us(int degrees) {1
  return degrees * (US_MAX - US_MIN) / 180 + US_MIN;
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

	Serial.println("Initializing servos...");
	servo1.attach(FRONT_SWIVEL);
	servo2.attach(FRONT_TILT);
	servo3.attach(BACK_SWIVEL);
	servo4.attach(BACK_TILT);

  Serial.println("Drive subsystem initialized");
}

void loop() {
	can.update();
	serial.update();
	sendData();
  delay(1);
}
