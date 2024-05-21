#include <Arduino.h>

class LedButton {
  private: 
    int ledPin;
    int buttonPin;
    bool flag = false;
  
  public: 
    LedButton(int ledPin, int buttonPin);
  
    void setup();
    void update();
    bool wasPressed();
    void turnOn();
    void turnOff();
};
