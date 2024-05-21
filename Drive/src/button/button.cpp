#include "button.h"

LedButton::LedButton(int ledPin, int buttonPin) : 
  ledPin(ledPin),
  buttonPin(buttonPin)
  { }

void LedButton::setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void LedButton::update() {
  if (digitalRead(buttonPin) == HIGH) flag = true;
}

bool LedButton::wasPressed() {
  bool result = flag;
  flag = false;
  return result;
}

void LedButton::turnOn() {
  digitalWrite(ledPin, HIGH);
}

void LedButton::turnOff() {
  digitalWrite(ledPin, LOW);
}

void Buttons::setup() {
  yellow.setup();
  green.setup();
}

void Buttons::update() {
  if (yellow.wasPressed()) {
		data.status = RoverStatus::RoverStatus_IDLE;
	} else if (green.wasPressed()) {
		data.status = RoverStatus::RoverStatus_MANUAL;
	}
}

void Buttons::handleCommand(DriveCommand command) {
  if (command.status == RoverStatus::RoverStatus_MANUAL) {
    green.turnOn();
    yellow.turnOff();
    data.status = RoverStatus::RoverStatus_MANUAL;
  } else if (command.status == RoverStatus::RoverStatus_IDLE) {
    green.turnOff();
    yellow.turnOn();
    data.status = RoverStatus::RoverStatus_IDLE;
  }
}
