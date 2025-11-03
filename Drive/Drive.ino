#include "pinouts.h"
#include "src/utils/BURT_utils.h"
#include "src/drive.pb.h"
#include "src/utils/rover.h"

#define DATA_SEND_INTERVAL 50  // ms
#define MOTOR_UPDATE_INTERVAL 10  // ms

const Version version = {major: 1, minor: 2};

const int errorPin = 9;

void handleCommand(const uint8_t* data, int length);

void handleCanMessage(const CanMessage& message);
void handleMotorOutput(const CanMessage& message) {
  motors.handleMotorOutput(message);
}

void onConnect();
void onDisconnect();

BurtSerial serial(Device::Device_DRIVE, handleCommand, DriveData_fields, DriveData_size);
BurtCan<Can3> roverCan({
  idStart: 0x01,
  onMessage: handleCanMessage,
  isRoverNetwork: true,
  device: Device::Device_DRIVE,
  version: version,
  onConnect: onConnect,
  onDisconnect: onDisconnect,
});

// AK motors send data in the format (0x29 << 8) | MOTOR_ID
//
// Since we want to capture data from all the motors, the filter is set to include
// all commands starting with the ID 0x2900 (not including motor ID 0)
//
// AK motors use extended IDs, so the extended frame mailbox is listened to
BurtCan<Can1> motorCan(0x2901, 0x29FF, handleMotorOutput, true);

void sendData();

void sendAppliedOutput();
void sendBatteryOutput();
void sendLEDData();
void sendSwivelData();
void sendMotorsData();

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
	temperatureSensor.update();
	buttons.update();
	voltageSensor.update();
}

void sendData() {
	sendAppliedOutput();
	sendBatteryOutput();
	sendLEDData();
	sendSwivelData();
	sendMotorsData();

	if (!roverCan.isRoverConnected()) {
		DriveData data = DriveData_init_zero;
		data.version = version;
		data.has_version = true;
		serial.send(&data);
		serial.send(&buttons.data);
		serial.send(&motors.data);
		serial.send(&cameras.data);
		serial.send(&led_strip.data);
		serial.send(&voltageSensor.data);
		serial.send(&temperatureSensor.data);
	}
}

void handleCanMessage(const CanMessage& message) {
	DriveCommand command = DriveCommand_init_zero;
	if (message.id == ROVER_DRIVE_SET_SPEEDS_FRAME_ID) {
		rover_drive_set_speeds_t setSpeedsMsg = {0};
		rover_drive_set_speeds_unpack(&setSpeedsMsg, message.buf, message.len);

		command.throttle = rover_drive_set_speeds_throttle_decode(setSpeedsMsg.throttle);
		command.left = rover_drive_set_speeds_left_speed_decode(setSpeedsMsg.left_speed);
		command.right = rover_drive_set_speeds_right_speed_decode(setSpeedsMsg.right_speed);
		command.set_left = setSpeedsMsg.should_set_left == 1;
		command.set_right = setSpeedsMsg.should_set_right == 1;
		command.set_throttle = setSpeedsMsg.should_set_throttle == 1;
	} else if (message.id == ROVER_DRIVE_SET_LED_FRAME_ID) {
		rover_drive_set_led_t setLedMsg = {0};
		rover_drive_set_led_unpack(&setLedMsg, message.buf, message.len);

		command.blink = setLedMsg.blink == 1 ? BoolState_YES : BoolState_NO;
		command.color = static_cast<ProtoColor>(setLedMsg.color);
	} else if (message.id == ROVER_DRIVE_SET_SWIVEL_FRAME_ID) {
		rover_drive_set_swivel_t setSwivelMsg = {0};
		rover_drive_set_swivel_unpack(&setSwivelMsg, message.buf, message.len);

		if (setSwivelMsg.set_front_swivel == 1) {
			command.front_swivel = rover_drive_set_swivel_front_swivel_decode(setSwivelMsg.front_swivel);
		}
		if (setSwivelMsg.set_front_tilt == 1) {
			command.front_tilt = rover_drive_set_swivel_front_tilt_decode(setSwivelMsg.front_tilt);
		}
		if (setSwivelMsg.set_rear_swivel == 1) {
			command.rear_swivel = rover_drive_set_swivel_rear_swivel_decode(setSwivelMsg.rear_swivel);
		}
		if (setSwivelMsg.set_rear_tilt == 1) {
			command.rear_tilt = rover_drive_set_swivel_rear_tilt_decode(setSwivelMsg.rear_tilt);
		}
	}

	buttons.handleCommand(command);
	motors.handleCommand(command);
	cameras.handleCommand(command);
	led_strip.handleCommand(command);
}

void handleCommand(const uint8_t* data, int length) {
	if (roverCan.isRoverConnected()) {
		return;
	}
	auto command = BurtProto::decode<DriveCommand>(data, length, DriveCommand_fields);
	buttons.handleCommand(command);
	motors.handleCommand(command);
	cameras.handleCommand(command);
	led_strip.handleCommand(command);
}

void onConnect() {
  digitalWrite(LED_BUILTIN, HIGH);
}

void onDisconnect() {
  digitalWrite(LED_BUILTIN, LOW);

  DriveCommand stopCommand = DriveCommand_init_zero;
  stopCommand.set_throttle = true;
  stopCommand.throttle = 0.0;
  motors.handleCommand(stopCommand);
  motors.handleMotorOutput(motorCan);
}

void sendAppliedOutput() {
	rover_drive_applied_output_data_t outputMessage = {0};
	outputMessage.throttle = rover_drive_applied_output_data_throttle_encode(motors.data.throttle);
	outputMessage.left_speed = rover_drive_applied_output_data_left_speed_encode(motors.data.left);
	outputMessage.right_speed = rover_drive_applied_output_data_right_speed_encode(motors.data.right);

	CanMessage message;
	message.id = ROVER_DRIVE_APPLIED_OUTPUT_DATA_FRAME_ID;
	message.len = rover_drive_applied_output_data_pack(message.buf, &outputMessage, message.len);
	roverCan.sendMessage(message);
}

void sendBatteryOutput() {
	rover_drive_battery_data_t batteryMessage = {0};
	batteryMessage.voltage = rover_drive_battery_data_voltage_encode(voltageSensor.data.battery_voltage);
	batteryMessage.temperature = rover_drive_battery_data_temperature_encode(temperatureSensor.data.battery_temperature);

	CanMessage message;
	message.id = ROVER_DRIVE_BATTERY_DATA_FRAME_ID;
	message.len = rover_drive_battery_data_pack(message.buf, &batteryMessage, message.len);
	roverCan.sendMessage(message);
}

void sendLEDData() {
	rover_drive_led_data_t ledMessage = {0};
	ledMessage.color = static_cast<uint8_t>(led_strip.data.color);

	CanMessage message;
	message.id = ROVER_DRIVE_LED_DATA_FRAME_ID;
	message.len = rover_drive_led_data_pack(message.buf, &ledMessage, message.len);
	roverCan.sendMessage(message);
}

void sendSwivelData() {
	rover_drive_swivel_data_t swivelMsg = {0};
	swivelMsg.front_swivel = rover_drive_swivel_data_front_swivel_encode(cameras.data.front_swivel);
	swivelMsg.front_tilt = rover_drive_swivel_data_front_tilt_encode(cameras.data.front_tilt);
	swivelMsg.rear_swivel = rover_drive_swivel_data_rear_swivel_encode(cameras.data.rear_swivel);
	swivelMsg.rear_tilt = rover_drive_swivel_data_rear_tilt_encode(cameras.data.front_tilt);

	CanMessage message;
	message.id = ROVER_DRIVE_SWIVEL_DATA_FRAME_ID;
	message.len = rover_drive_swivel_data_pack(message.buf, &swivelMsg, message.len);
	roverCan.sendMessage(message);
}

void sendMotorsData() {
	sendMotorData(DriveMotor::DriveMotor_FRONT_LEFT, &motors.data.front_left_motor);
	sendMotorData(DriveMotor::DriveMotor_MIDDLE_LEFT, &motors.data.middle_left_motor);
	sendMotorData(DriveMotor::DriveMotor_BACK_LEFT, &motors.data.back_left_motor);
	sendMotorData(DriveMotor::DriveMotor_FRONT_RIGHT, &motors.data.front_right_motor);
	sendMotorData(DriveMotor::DriveMotor_MIDDLE_RIGHT, &motors.data.middle_right_motor);
	sendMotorData(DriveMotor::DriveMotor_BACK_RIGHT, &motors.data.back_right_motor);
}

void sendMotorData(DriveMotor motor, DriveMotorData* dataProto) {
	rover_drive_motor_data_t motorData = {0};

	motorData.motor_value = static_cast<uint8_t>(motor);
	motorData.speed = rover_drive_motor_data_speed_encode(dataProto->speed);
	motorData.current = rover_drive_motor_data_current_encode(dataProto->current);
	motorData.temperature = rover_drive_motor_data_temperature_encode(dataProto->temperature);
	motorData.error_code = static_cast<uint8_t>(dataProto->error);

	CanMessage message;
	message.id = ROVER_DRIVE_MOTOR_DATA_FRAME_ID;
	message.len = rover_drive_motor_data_pack(message.buf, &motorData, message.len);
	roverCan.sendMessage(message);
}
