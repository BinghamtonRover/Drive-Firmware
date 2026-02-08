#include "../drive.pb.h"
#include "../utils/BURT_timer.h"
#include <FastLED.h>

const int blinkInterval = 250;  // ms

#define NUM_LEDS 30
#define DATA_PIN 14
#define CLOCK_PIN 15

enum class BlinkMode {
    NONE,
    BLINK_RED,
    BLINK_GREEN,
    ALTERNATING
};
 
/// The RGB LED strip that goes around the rover.
class LedStrip {
  private:
    // /// Whether to light the red LEDs.
    // bool redValue;

    // /// Whether to light the green LEDs.
    // bool greenValue;

    // /// Whether to light the blue LEDs.
    // bool blueValue;

    // /// Whether the LED strip is currently lit during this phase of blinking.
    // ///
    // /// When #shouldBlink is true, #update will alternate this value to be on and off
    // /// and light the LED strip accordingly.
    // bool isOn;

    // /// What the old color of the
    // ProtoColor oldColor = ProtoColor::ProtoColor_UNLIT;


    // USING FAST LED (PATRICK BROWN)

    CRGB leds[NUM_LEDS];

    BurtTimer blinkTimer{blinkInterval};
    bool toggle = false;
    BlinkMode currentMode = BlinkMode::NONE;


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

    void blinkRed();
    void blinkGreen();
    void blinkAlternating();

    void applyColor(const CRGB& color);

    /// Handles commands to change the LED strip.
    void handleCommand(DriveCommand command);
};
