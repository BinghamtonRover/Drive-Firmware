#include <Servo.h>
#include "../drive.pb.h"

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
