#include "../drive.pb.h"
#include "../utils/BURT_can.h"

const int maxRpm = 40'000;

const int leftMotor1 = 0x306;
const int leftMotor2 = 0x307;
const int leftMotor3 = 0x301;
const int rightMotor1 = 0x303;
const int rightMotor2 = 0x308;
const int rightMotor3 = 0x304;

/// A wrapper class to control all the motors.
///
/// The rover has tank-like or skid-steer controls. The wheels do not physically rotate, but rather
/// the different sides are spun at different rates to turn either in place or while driving. This
/// class drives each side as a whole unit, with #setSpeeds representing the speeds of both sides.
///
/// It is much easier to reason about steering if you only have to worry about the speeds of each
/// side relative to each other, and not their total speed. For example, if one wants to turn left
/// in place, they should spin the left wheels backwards and the right wheels forward. However,
/// doing this will cause the rover to turn with the wheels going at #maxRpm! To prevent this,
/// #throttle is used to cap total speed of the wheels with respect to #maxRpm.
///
/// So, between the wheel `speed` and a given `throttle`, the wheel will end up spinning at:
///
/// ```
/// maxRpm * throttle * speed
/// ```
class Motors {
  private:
    /// The CAN data to send to the left motors.
    ///
    /// See section 5.1.4 of the AK-70 motor docs: https://store.tmotor.com/images/file/202208/251661393360838805.pdf
    uint8_t leftBuffer[4];

    /// The CAN data to send to the right motors.
    ///
    /// See section 5.1.4 of the AK-70 motor docs: https://store.tmotor.com/images/file/202208/251661393360838805.pdf
    uint8_t rightBuffer[4];

    /// Recalculates the given #buffer with the new #speed and #throttle.
    void updateBuffer(float speed, uint8_t* buffer);

    /// Calls #updateBuffer for each CAN buffer.
    void updateBuffers();

  public:
    /// The current state of the motors.
    DriveData data;

    /// How fast the left wheels are driving, as a percentage of #throttle (range [0, 1]).
    float left = 0;

    /// How fast the right wheels are driving, as a percentage of #throttle (range [0, 1]).
    float right = 0;

    /// How fast any wheel can drive, as a percentage of #maxRpm (range [0, 1]).
    float throttle = 0;

    /// Initializes the motors as needed.
    void setup();

    /// Sends CAN commands to each motor to reflect the current speed and throttle.
    void sendMotorCommands(BurtCan<Can1> &can);

    /// Parses and records output from a given motor.
    void handleMotorOutput(const uint8_t* data, int length);

    /// Handles a command to change speeds from the Dashboard.
    void handleCommand(DriveCommand command);

    /// Stops all the motors, setting speeds and throttle to 0.
    void stop();

    /// Sets the speeds of the left and right sides, as percentages from 0.0 to 1.0.
    ///
    /// Note: This won't take effect physically until #sendMotorCommands is called.
    void setSpeeds(float newLeft, float newRight);

    /// Sets the maximum throttle, as a percentage in the range [0, 1].
    ///
    /// Note: This won't take effect physically until #sendMotorCommands is called.
    void setThrottle(float newThrottle);
};
