#include <Arduino.h>
#include "../drive.pb.h"

const int greenLedPin = 3;
const int greenButtonPin = 4;
const int yellowLedPin = 1;
const int yellowButtonPin = 2;

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

class Buttons {
  private: 
    LedButton yellow = LedButton(yellowLedPin, yellowButtonPin);
    LedButton green = LedButton(greenLedPin, greenButtonPin);

  public: 
    DriveData data;
    void setup();
    void update();
    void handleCommand(DriveCommand command);
};
