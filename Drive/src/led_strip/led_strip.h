#include "../drive.pb.h"
#include "../utils/BURT_timer.h"

const int blinkInterval = 250;  // ms

class LedStrip {
  private:
    int redValue;
    int greenValue;
    int blueValue;
    bool isOn;
    ProtoColor oldColor = ProtoColor::ProtoColor_UNLIT;
  
  public: 
    DriveData data;
    void setup();
    
    void red();
    void green();
    void blue();
    void off();
    bool shouldBlink = false;

    void update();
    void handleCommand(DriveCommand command);
};
