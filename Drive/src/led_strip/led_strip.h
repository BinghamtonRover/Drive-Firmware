#include "../drive.pb.h"
#include "../utils/BURT_timer.h"

const int blinkInterval = 250;  // ms

class LedStrip {
  private:
    int redValue;
    int greenValue;
    int blueValue;
    bool shouldBlink = false;
    bool isOn;
  
  public: 
    DriveData data;
    void setup();
    
    void red();
    void green();
    void blue();
    void off();

    void update();
    void handleCommand(DriveCommand command);
};
