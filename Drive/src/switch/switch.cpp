#include "switch.h"

void Relay::setup() {
    pinMode(relayPin, OUTPUT);
    pinMode(ledPin, OUTPUT);
}

void Relay::update() {
    if (softwareState) {
        turnOn();
    } else {
        turnOff();
    }
}

void Relay::turnOn() { 
    digitalWrite(relayPin, HIGH);
    digitalWrite(ledPin, HIGH);
    relayData = BoolState_ON;
}

void Relay::turnOff() {
    digitalWrite(relayPin, LOW);
    digitalWrite(ledPin, LOW);
    relayData = BoolState_OFF;
}

void Relay::handleCommand(BoolState command) { 
    switch (command) {
        case BoolState_ON:
            softwareState = true;
            break;
        case BoolState_OFF:
            softwareState = false;
            break;
        case BoolState_BOOL_UNDEFINED:
            break;
    }
}

void Relays::setup() {
    delay(250);

    backLeftMotor.setup();
    Serial.println("Back left motor relay initialized");
    delay(250);

    backRightMotor.setup();
    Serial.println("Back right motor relay initialized");
    delay(250);

    frontLeftMotor.setup();
    Serial.println("Front left motor relay initialized");
    delay(250);
    
    frontRightMotor.setup();
    Serial.println("Front right motor relay initialized");
    delay(250);
    
    arm.setup();
    Serial.println("Arm relay initialized");
    delay(250);
    
    science.setup();
    Serial.println("Science relay initialized");
    delay(250);
    
    drive.setup();
    Serial.println("Drive relay initialized");
    delay(250);
    
}

void Relays::update() {
    backLeftMotor.update();
    backRightMotor.update();
    frontLeftMotor.update();
    frontRightMotor.update();
    arm.update();
    science.update();
    drive.update();
}

void Relays::handleCommand(RelaysCommand command) {
    backLeftMotor.handleCommand(command.backLeftMotor);
    backRightMotor.handleCommand(command.backRightMotor);
    frontLeftMotor.handleCommand(command.frontLeftMotor);
    frontRightMotor.handleCommand(command.frontRightMotor);
    arm.handleCommand(command.arm);
    science.handleCommand(command.science);
    drive.handleCommand(command.drive);
}

// void OverrideSwitch::setup() {
//     pinMode(switchPin, INPUT);
// }