#include <Arduino.h>
#include "LockerService.h"

// void IRAM_ATTR buttonPressInterrupt() {
// 	run = true;
// }

LockerService lockerService;

void setup() {
	Serial.begin(115200);
	pinMode(BUTTON_PIN, INPUT);
}

void loop() {
	lockerService.poll();
}
