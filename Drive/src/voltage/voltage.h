#include "../drive.pb.h"

/// A PWM-based voltage sensor.
class VoltageSensor {
  private:
    /// The PWM pin this sensor is attached to.
    int pin;

  public:
    /// The current state of the sensor.
    DriveData data;

    VoltageSensor(int pin);

    /// Sets up the sensor for reading.
    void setup();

    /// Updates #data with the value from #read.
    void update();

    /// Reads the voltage of the sensor.
    float read();
};
