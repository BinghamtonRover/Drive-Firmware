#ifndef SWITCH_H
#define SWITCH_H

#include <Arduino.h>
#include "../relays.pb.h"

/// @brief Define the Teensy pins for the relays [Subject to change]
// Drive Motors
const int bLeftRelayPin = 23;
const int bLeftLedPin = 2;
const int bRightRelayPin = 22;
const int bRightLedPin = 3;
const int fLeftRelayPin = 21;
const int fLeftLedPin = 4;
const int fRightRelayPin = 20;
const int fRightLedPin = 5;

// Arm, Science, and Drive
const int armRelayPin = 37;
const int armLedPin = 28;
const int scienceRelayPin = 36;
const int scienceLedPin = 29;
const int driveRelayPin = 35;
const int driveLedPin = 30;

/// @brief Class to represent a relay on the relay board
///
/// Use this class to represent a relay on the relay board. It reads the
/// physical state of the switch as well as the state specified by the software
/// and updates the state of the relay accordingly.
///
/// Fields:
///     relayPin: The number of the Teensy pin that the relay is connected to
///     ledPin: The number of the Teensy pin that the LED is connected to
///     relayData: The state of the relay (ON, OFF, or BOOL_UNDEFINED)
///     softwareState: The state of the relay specified by the software, initially false
class Relay {
    private: 
        int relayPin;
        int ledPin;
        BoolState relayData;
        bool softwareState = false;        

    public: 
        /// @brief Constructs a Relay object with specified output pin.
        /// @param outputPin The Teensy pin that the relay is connected to.
        /// @param ledPin The Teensy pin that the LED is connected to.
        Relay(int relayPin, int ledPin) : 
            relayPin(relayPin), 
            ledPin(ledPin)
            { }
        
        /// @brief Set up the relay by setting the pin mode
        /// Call for each relay in the setup() function of the Relays class
        void setup();

        /// @brief Update the relay by turning it on or off based on the software state
        void update();  

        /// @brief Turn on the relay and LED and set the relay data to ON
        /// Call this if software state is true
        void turnOn();

        /// @brief Turn off the relay and LED and set the relay data to OFF
        /// Call this if software state is false
        void turnOff();

        /// @brief Handle a command by setting the software state
        /// @param command the command to be handled (ON, OFF, or BOOL_UNDEFINED)
        /// Call this when handling a command to update the software state of the relay
        void handleCommand(BoolState command);
};

/// Class to represent all the relays on the relay board
///
/// Use this class to represent all the relays on the relay board. It contains
/// instances of the Relay class for each relay and handles commands to
/// update the state of the relays.
class Relays {
    private:
        /// @brief Instantiate all RelaySwitch objects
        // Drive Motors
        Relay backLeftMotor = Relay(bLeftRelayPin, bLeftLedPin);
        Relay backRightMotor = Relay(bRightRelayPin, bRightLedPin);
        Relay frontLeftMotor = Relay(fLeftRelayPin, fLeftLedPin);
        Relay frontRightMotor = Relay(fRightRelayPin, fRightLedPin);

        // Arm, Science, and Drive
        Relay arm = Relay(armRelayPin, armLedPin);
        Relay science = Relay(scienceRelayPin, scienceLedPin);
        Relay drive = Relay(driveRelayPin, driveLedPin);

    public: 
        /// @brief The data from each relay to be sent to the dashboard
        /// ON, OFF, or BOOL_UNDEFINED
        /// Initialized to BOOL_UNDEFINED for all relays
        RelaysData data = RelaysData_init_default;

        /// @brief Set up the relays by calling the setup() function of each relay
        /// Call this function in the setup() function of the main sketch
        void setup();

        /// @brief Update the relays by calling the update() function of each relay
        /// Call this function in the loop() function of the main sketch
        /// after the update() function of the serial and timer objects
        void update();

        /// @brief Handle a command by calling the handleCommand() function of each relay
        /// @param command the command to be handled
        /// Sends each relay the relevant command to update the relay's software state
        void handleCommand(RelaysCommand command);
};

#endif