#include "../drive.pb.h"

class TemperatureSensor {
  private:
    int pin;
    
  public:
    DriveData data;
    TemperatureSensor(int pin);
    void setup();
    void update();
};
