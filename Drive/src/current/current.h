#ifndef VOLTAGE_H
#define VOLTAGE_H

#include <Arduino.h>

#include "../drive.pb.h"


class CurrentSensor {
private:
	int pin;

public:

	DriveData data = DriveData_init_default;
	
	CurrentSensor(int pin);

	void setup();

	float read();

	void update();

};

#endif