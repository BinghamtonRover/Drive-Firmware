#include "../drive.pb.h"
#include "../utils/BURT_can.h"

class Motors {
  private: 
    uint8_t leftBuffer[4];
    uint8_t rightBuffer[4];
    void updateBuffer(float speed, uint8_t* buffer);
    void updateBuffers();
  
  public:
    DriveData data;
    float left = 0;
    float right = 0;
    float throttle = 0; 
  
  
    void sendMotorCommands(BurtCan<Can1> &can);
    void handleMotorOutput(const uint8_t* data, int length);
    void handleCommand(DriveCommand command);

    void stop();
    void setSpeeds(float newLeft, float newRight);
    void setThrottle(float newThrottle);
};
