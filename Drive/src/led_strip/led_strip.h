#include "../drive.pb.h"
#include "../utils/BURT_timer.h"

const int blinkInterval = 250;  // ms

const int redPin = 14;
const int greenPin = 15;
const int bluePin = 16;

/// The RGB LED strip that goes around the rover.
class LedStrip {
  private:
    /// Whether to light the red LEDs.
    bool redValue;

    /// Whether to light the green LEDs.
    bool greenValue;

    /// Whether to light the blue LEDs.
    bool blueValue;

    /// Whether the LED strip is currently lit during this phase of blinking.
    ///
    /// When #shouldBlink is true, #update will alternate this value to be on and off
    /// and light the LED strip accordingly.
    bool isOn;

    /// What the old color of the
    ProtoColor oldColor = ProtoColor::ProtoColor_UNLIT;

  public:
    /// The current status of the LED strip.
    DriveData data;

    /// Sets up the LED strip.
    void setup();

    /// Updates the LED strip according to the latest commands and #shouldBlink.
    void update();

    /// Turns the LED strip red.
    void red();

    /// Turns the LED strip red.
    void green();

    /// Turns the LED strip red.
    void blue();

    /// Turns the LED strip off.
    void off();

    /// Whether the LED strip should be blinking.
    bool shouldBlink = false;

    /// Handles commands to change the LED strip.
    void handleCommand(DriveCommand command);
};
