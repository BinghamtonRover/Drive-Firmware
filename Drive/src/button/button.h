#include <Arduino.h>
#include "../drive.pb.h"

const int greenLedPin = 2;
const int greenButtonPin = 7;
const int yellowLedPin = 3;
const int yellowButtonPin = 6;

/// An button that has an LED built into it.
///
/// - Be sure to call #setup and #update appropriately
/// - You can check #wasPressed to see if the button was pressed last update
/// - Use #turnOn and #turnOff to control the LED
class LedButton {
  private:
    /// The pin number of the LED.
    int ledPin;

    /// The pin number of the button.
    int buttonPin;

    /// Whether the button was pressed since #wasPressed was last checked.
    bool flag = false;

  public:
    /// Creates an LED button object.
    LedButton(int ledPin, int buttonPin);

    /// Sets the LED pin to output and the button pin to input.
    void setup();

    /// Monitors the status of the button.
    void update();

    /// Returns whether the button was pressed since this function was last called.
    bool wasPressed();

    /// Turns the LED on.
    void turnOn();

    /// Turns the LED off.
    void turnOff();
};

/// A collection of #LedButtons that are mutually exclusive.
///
/// The rover has three LED buttons: red, yellow, and green. Red is used to control power to the
/// Raspberry Pis, so it is electrically tied to their reset buttons and therefore not used in the
/// firmware. The yellow and green buttons represent idle mode and active mode, respectively.
/// When one  button is pressed, the other(s) will turn off and the mode will be transmitted to
/// the Subsystems, which can decide how to react and stream their status to the Dashboard.
///
/// These buttons are used to indicate whether it is safe to manipulate the rover. WHen the green
/// light is on, the rover can theoretically start moving at any time, causing damage or injury.
/// When the green light is on, the rover will ignore all commands sent to it and is therefore
/// safe to perform basic mechanical maintenance on. However, the rover may still be electrically
/// powered and extreme care should be taken when handling its electrical components.
class Buttons {
  private:
    /// The yellow button which represents idle mode.
    LedButton yellow = LedButton(yellowLedPin, yellowButtonPin);

    /// The green button which represents active mode.
    LedButton green = LedButton(greenLedPin, greenButtonPin);

  public:
    /// The data to send to the Dashboard about the buttons.
    DriveData data;

    /// Sets up all the buttons as needed.
    void setup();

    /// Updates the buttons and their LEDs and updates #data.
    void update();

    /// Handles commands to turn on the green or yellow LEDs.
    void handleCommand(DriveCommand command);
};
