class VoltageSensor {
  private: 
    int pin;

  public:
    VoltageSensor(int pin);
    void setup();
    float read();
};
