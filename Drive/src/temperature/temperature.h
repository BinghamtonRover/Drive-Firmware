#include "../drive.pb.h"

/// A PWM-based temperature sensor.
class TemperatureSensor {
  private:
    /// The PWM pin this sensor is attached to.
    int pin;

  public:
    /// The current state of the temperature sensor.
    DriveData data;

    TemperatureSensor(int pin);

    /// Initializes the sensor for reading.
    void setup();

    /// Updates #data with the current value from #read.
    void update();

    /// Reads the temperature of the sensor.
    float read();
};
