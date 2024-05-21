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
