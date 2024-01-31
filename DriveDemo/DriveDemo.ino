#include "src/utils/BURT_utils.h"
#include "src/BURT_vesc.h"
#include "src/drive.pb.h"

#define DRIVE_COMMAND_ID 0x053
#define DRIVE_DATA_ID 0x690

// example of how to send velocity over CAN to motor with ID of 6
//  hex(MOTOR_ID | (3 << 8))
// == '0x306' 

// left
#define DRIVE_MOTOR_0_ID 0x302
#define DRIVE_MOTOR_1_ID 0x303
#define DRIVE_MOTOR_2_ID 0x307

// right
#define DRIVE_MOTOR_3_ID 0x308
#define DRIVE_MOTOR_4_ID 0x304
#define DRIVE_MOTOR_5_ID 0x301

#define MAX_DRIVE_SPEED 10000


float leftVelocity, rightVelocity, throttle;
int canSendInterval = 5;
unsigned long nextSendTime;

uint8_t* decimalToHexArray(int decimalSpeed) {
  if (decimalSpeed < 5 && decimalSpeed > 0) {
    decimalSpeed = 0;
  } else if (decimalSpeed > -5 && decimalSpeed < 0) {
    decimalSpeed = 0;
  }
  uint8_t* speed = new uint8_t[4];
  speed[0] = (decimalSpeed & 0xFF000000) >> 24;
  speed[1] = (decimalSpeed & 0x00FF0000) >> 16;
  speed[2] = (decimalSpeed & 0x0000FF00) >> 8;
  speed[3] = (decimalSpeed & 0x000000FF);
  return speed;
}

void handleDriveCommand(const uint8_t* data, int length) {
  Serial.println("handling command");
	auto command = BurtProto::decode<DriveCommand>(data, length, DriveCommand_fields);
  Serial.print("set_throttle: ");
  Serial.println(command.set_throttle);
  Serial.print("set_left: ");
  Serial.println(command.set_left);
  Serial.print("set_right: ");
  Serial.println(command.set_right);
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
}

// BurtSerial serial(handleDriveCommand, Device::Device_DRIVE);
BurtCan can(DRIVE_COMMAND_ID, handleDriveCommand);

void sendToMotors() {
  if (millis() < nextSendTime) return;
  // send to all 6 motors
  can.sendRaw(DRIVE_MOTOR_0_ID, decimalToHexArray(MAX_DRIVE_SPEED * leftVelocity * throttle), 4);
  can.sendRaw(DRIVE_MOTOR_1_ID, decimalToHexArray(MAX_DRIVE_SPEED * leftVelocity * throttle), 4);
  can.sendRaw(DRIVE_MOTOR_2_ID, decimalToHexArray(MAX_DRIVE_SPEED * leftVelocity * throttle), 4);
  
  can.sendRaw(DRIVE_MOTOR_3_ID, decimalToHexArray(MAX_DRIVE_SPEED * rightVelocity * throttle), 4);
  can.sendRaw(DRIVE_MOTOR_4_ID, decimalToHexArray(MAX_DRIVE_SPEED * rightVelocity * throttle), 4);
  can.sendRaw(DRIVE_MOTOR_5_ID, decimalToHexArray(MAX_DRIVE_SPEED * rightVelocity * throttle), 4);

  nextSendTime = millis() + canSendInterval;
}

void setup() {
	Serial.begin(9600);
  Serial.println("Initializing Drive subsystem");

  Serial.println("Initializing communications...");
	can.setup();
  nextSendTime = millis() + canSendInterval;

  Serial.println("Drive subsystem initialized");
}

void loop() {
	can.update();
  // throttle = 0.5;
  // leftVelocity = 0.5;
  // rightVelocity = 0.5;
  sendToMotors();

  // delay(1);
}
