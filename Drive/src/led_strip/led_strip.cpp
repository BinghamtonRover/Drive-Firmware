
#include <Arduino.h>

#include "led_strip.h"

const int redPin = 14;
const int greenPin = 15;
const int bluePin = 16;

void LedStrip::setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  blue();
}

void LedStrip::red() {
  redValue = HIGH;
  greenValue = LOW;
  blueValue = LOW;
  data.color = ProtoColor::ProtoColor_RED;
  oldColor = ProtoColor::ProtoColor_RED;
}

void LedStrip::green() {
  redValue = LOW;
  greenValue = HIGH;
  blueValue = LOW;
  data.color = ProtoColor::ProtoColor_GREEN;
  oldColor = ProtoColor::ProtoColor_GREEN;
}

void LedStrip::blue() {
  redValue = LOW;
  greenValue = LOW;
  blueValue = HIGH;
  data.color = ProtoColor::ProtoColor_BLUE;
  oldColor = ProtoColor::ProtoColor_BLUE;
}

void LedStrip::off() {
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);
  data.color = ProtoColor::ProtoColor_UNLIT;
}

void LedStrip::update() {
  if (shouldBlink && !isOn) {
    off();
  } else {
    digitalWrite(redPin, redValue);
    digitalWrite(greenPin, greenValue);
    digitalWrite(bluePin, blueValue);  
    data.color = oldColor;
  }
  isOn = !isOn;
}

void LedStrip::handleCommand(DriveCommand command) {
  switch (command.color) {
    case ProtoColor::ProtoColor_PROTO_COLOR_UNDEFINED: break;
    case ProtoColor::ProtoColor_RED: red(); break;
    case ProtoColor::ProtoColor_GREEN: green(); break;
    case ProtoColor::ProtoColor_BLUE: blue(); break;
    case ProtoColor::ProtoColor_UNLIT: off(); break;
  }
  switch (command.blink) {
    case BoolState::BoolState_BOOL_UNDEFINED: break;
    case BoolState::BoolState_ON: shouldBlink = true; break;
    case BoolState::BoolState_OFF: shouldBlink = false; break;
  }
}

