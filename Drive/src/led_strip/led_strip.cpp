#include <Arduino.h>

#include "led_strip.h"

void LedStrip::setup() {
  // pinMode(redPin, OUTPUT);
  // pinMode(greenPin, OUTPUT);
  // pinMode(bluePin, OUTPUT);
  // blue();
  FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();
  blue();
}

// void LedStrip::applyColor(CRGB color) {
//   analogWrite(redPin, color.r);
//   analogWrite(greenPin, color.g);
//   analogWrite(bluePin, color.b);
// }

void LedStrip::applyColor(const CRGB& color) {
  fill_solid(leds, NUM_LEDS, color);
  FastLED.show();
}

void LedStrip::red() {

  shouldBlink = false;
  currentMode = BlinkMode::NONE;
  applyColor(CRGB::Red);


  // redValue = HIGH;
  // greenValue = LOW;
  // blueValue = LOW;
  data.color = ProtoColor::ProtoColor_RED;
  // oldColor = ProtoColor::ProtoColor_RED;
}

void LedStrip::green() {

  shouldBlink = false;
  currentMode = BlinkMode::NONE;
  applyColor(CRGB::Green);


  // redValue = LOW;
  // greenValue = HIGH;
  // blueValue = LOW;
  data.color = ProtoColor::ProtoColor_GREEN;
  // oldColor = ProtoColor::ProtoColor_GREEN;
}

void LedStrip::blue() {

  shouldBlink = false;
  currentMode = BlinkMode::NONE;
  applyColor(CRGB::Blue);

  // redValue = LOW;
  // greenValue = LOW;
  // blueValue = HIGH;
  data.color = ProtoColor::ProtoColor_BLUE;
  // oldColor = ProtoColor::ProtoColor_BLUE;
}

void LedStrip::off() {

  shouldBlink = false;
  currentMode = BlinkMode::NONE;
  applyColor(CRGB::Black);

  // digitalWrite(redPin, LOW);
  // digitalWrite(greenPin, LOW);
  // digitalWrite(bluePin, LOW);
  data.color = ProtoColor::ProtoColor_UNLIT;
}

// NEW BLINKING METHODS
void LedStrip::blinkRed() {
  currentMode = BlinkMode::BLINK_RED;
  shouldBlink = true;
}

void LedStrip::blinkGreen() {
  currentMode = BlinkMode::BLINK_GREEN;
  shouldBlink = true;
}

void LedStrip::blinkAlternating() {
  currentMode = BlinkMode::BLINK_ALTERNATING;
  shouldBlink = true;
}

// void LedStrip::update() {
//   if (shouldBlink && !isOn) {
//     off();
//   } else {
//     digitalWrite(redPin, redValue);
//     digitalWrite(greenPin, greenValue);
//     digitalWrite(bluePin, blueValue);
//     data.color = oldColor;
//   }
//   isOn = !isOn;
// }

// NEW UPDATE METHOD
void LedStrip::update() {
  if (!shouldBlink || currentMode == BlinkMode::NONE) {
    return;
  }

  if (blinkTimer.hasExpired()) {
    toggle = !toggle;
    blinkTimer.reset();
  }

  switch (currentMode) {
    case BlinkMode::BLINK_RED:
      applyColor(toggle ? CRGB::Red : CRGB::Black);
      data.color = ProtoColor::ProtoColor_RED;
      break;

    case BlinkMode::BLINK_GREEN:
      applyColor(toggle ? CRGB::Green : CRGB::Black);
      data.color = ProtoColor::ProtoColor_GREEN;
      break;

    case BlinkMode::ALTERNATING:
      applyColor(toggle ? CRGB::Red : CRGB::Green);
      data.color = ProtoColor::ProtoColor_UNLIT;
      break;

    // case BoolState::BoolState_OFF:
    //   shouldBlink = false;
    //   currentMode = BlinkMode::NONE;
    //   applyColor(CRGB::Black);
    //   break;

    default:
      break;
  }
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

