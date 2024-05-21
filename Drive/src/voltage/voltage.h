#include "../drive.pb.h"

class VoltageSensor {
  private: 
    int pin;

  public:
    DriveData data;
    VoltageSensor(int pin);
    void setup();
    void update();
    float read();
};
