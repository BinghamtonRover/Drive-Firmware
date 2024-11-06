#include <Servo.h>
#include "../drive.pb.h"

const int frontSwivelPin = 33;
const int frontTiltPin = 36;
const int backSwivelPin = 4;
const int backTiltPin = 5;

/// A collection of servos that control the rover's camera system.
class Cameras {
  private:
    Servo frontSwivel;
    Servo frontTilt;
    Servo backSwivel;
    Servo backTilt;

  public:
    DriveData data;
    void setup();
    void handleCommand(DriveCommand command);
};
