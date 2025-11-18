// #include <Servo.h>

#include "pinouts.h"
#include "src/utils/BURT_utils.h"
#include "src/control_board.pb.h"

#define DRIVE_COMMAND_ID   0x53
#define DRIVE_DATA_ID      0x14

#define DATA_SEND_INTERVAL 50  // ms (relays was 250 ms, drive was 50ms, so chose the smaller one)
#define MOTOR_UPDATE_INTERVAL 10  // ms

const Version version = {major: 1, minor: 2};

const int errorPin = 9;

void handleCommand(const uint8_t* data, int length);

void handleMotorOutput(const CanMessage& message) {
  motors.handleMotorOutput(message);
}

BurtSerial serial(Device::Device_CONTROL_BOARD, handleCommand, ControlBoardData_fields, ControlBoardData_size);

// AK motors send data in the format (0x29 << 8) | MOTOR_ID
//
// Since we want to capture data from all the motors, the filter is set to include
// all commands starting with the ID 0x2900 (not including motor ID 0)
//
// AK motors use extended IDs, so the extended frame mailbox is listened to
BurtCan<Can1> motorCan(0x2901, 0x29FF, handleMotorOutput, true);

void sendData();
void updateMotors() { motors.sendMotorCommands(motorCan); }
void updateLedStrip() { led_strip.update(); }

BurtTimer dataTimer(DATA_SEND_INTERVAL, sendData);
BurtTimer motorTimer(MOTOR_UPDATE_INTERVAL, updateMotors);
BurtTimer blinkTimer(blinkInterval, updateLedStrip);


void setup() {
  pinMode(errorPin, OUTPUT);
	Serial.begin(9600);
	Serial.println("Initializing Drive subsystem");
	Serial.println("Initializing software...");
	motorCan.setup();
	serial.setup();
	dataTimer.setup();
	motorTimer.setup();
	blinkTimer.setup();

	relays.setup();
	voltageSensor.setup();


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
	motorCan.update();
	dataTimer.update();
	motorTimer.update();
	blinkTimer.update();
	temperatureSensor.update();
	buttons.update();
	voltageSensor.update();
	relays.update();
}


void sendData() {
  DriveData d_data = DriveData_init_zero;
  Relaydata r_data = RelayData_init_zero;

  ControlBoardData c_data = ControlBoardData_init_zero;

  c_data.relays = r_data;
  data.version = version;
	data.has_version = true;
 serial.send(&data);
	serial.send(&buttons.data);
	serial.send(&motors.data);
	serial.send(&cameras.data);
	serial.send(&led_strip.data);
	serial.send(&voltageSensor.data);
	serial.send(&temperatureSensor.data);
	serial.send(&relays.data);

}

void handleCommand(const uint8_t* data, int length) {
	auto command = BurtProto::decode<ControlBoardCommand>(data, length, ControlBoardCommand_fields);
	buttons.handleCommand(command.Drive);
	motors.handleCommand(command.Drive);
	cameras.handleCommand(command.Drive);
	led_strip.handleCommand(command.Drive);

	relays.handleCommand(command.Relays);
}
