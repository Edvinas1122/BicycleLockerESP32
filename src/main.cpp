#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <Display.h>
#include <Network.h>
#include "env.h"
#include "Locker.h"


bool run = false;

void onConnect() {
	run = true;
}

void onDisconnect() {
	run = false;
}

Display display;

void loggingMethod(const char *message) {
	Serial.println(message);
	display.displayText(message);
}

Network	localNetwork(
			LOCAL_SSID,
			LOCAL_USER,
			LOCAL_PASSWORD,
			loggingMethod,
			onConnect,
			onDisconnect
		);

void setup()
{
	Serial.begin(115200);
	display.init();
	localNetwork.init();
}

void loop()
{
	// vTaskDelay(portMAX_DELAY);
	if (run) {
		try {
			handleNetworkTasks(&run, &display);
		} catch (...) {}
	}
}