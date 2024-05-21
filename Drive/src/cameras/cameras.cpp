#include <Arduino.h>
#include "cameras.h"

const int frontSwivelPin = 4;
const int frontTiltPin = 5;
const int backSwivelPin = 33;
const int backTiltPin = 36;

void Cameras::setup() {
  frontSwivel.attach(frontSwivelPin);
  delay(200);
  frontTilt.attach(frontTiltPin);
  delay(200);
  backSwivel.attach(backSwivelPin);
  delay(200);
  backTilt.attach(backTiltPin);
}

void Cameras::handleCommand(DriveCommand command) {
	if (command.front_swivel != 0) {
    frontSwivel.write(command.front_swivel);
    data.front_swivel = command.front_swivel;
  }
	if (command.front_tilt != 0) {
    frontTilt.write(command.front_tilt);
    data.front_tilt = command.front_tilt;
  }
	if (command.rear_swivel != 0) {
    backSwivel.write(command.rear_swivel);
    data.rear_swivel = command.rear_swivel;
  }
	if (command.rear_tilt != 0) {
    backTilt.write(command.rear_tilt); 
    data.rear_tilt = command.rear_tilt;
  }
}
