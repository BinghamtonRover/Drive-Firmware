#include "../drive.pb.h"
#include "../utils/BURT_can.h"

const int maxRpm = 40'000;

const int leftMotor1 = 0x306;
const int leftMotor2 = 0x307;
const int leftMotor3 = 0x301;
const int rightMotor1 = 0x303;
const int rightMotor2 = 0x308;
const int rightMotor3 = 0x304;

class Motors {
  private:
    void updateBuffer(float speed, uint8_t* buffer);
    void updateBuffers();

  public:
    uint8_t leftBuffer[4];
    uint8_t rightBuffer[4];
    DriveData data;
    float left = 0;
    float right = 0;
    float throttle = 0;

    void setup();
    void sendMotorCommands(BurtCan<Can1> &can);
    void handleMotorOutput(const uint8_t* data, int length);
    void handleCommand(DriveCommand command);

    void stop();
    void setSpeeds(float newLeft, float newRight);
    void setThrottle(float newThrottle);
};
