#include <freertos/FreeRTOS.h>
#include <Display.h>
#include <Network.h>
#include "env.h"
#include "LockerService.h"
#include <Pusher.h>
#include <HTTPFetch.h>

bool run = false;

Network	localNetwork(
			LOCAL_SSID,
			LOCAL_USER,
			LOCAL_PASSWORD,
			[](const char* message) {Serial.print(message);},
			[]() {run = true;},
			[]() {run = false; WiFi.disconnect(true);}
		);

PusherService webSocketService(
	PUSHER_KEY,
	PUSHER_CLUSTER,
	[](const char* message) {Serial.print(message);}
);

void registerHandlers(
	PusherService &webSocketService,
	LockerService &lockerService,
	Display *display,
	const char *mainChannel
);
void autoSubscribeToChannel(
	HTTPInterface &interface,
	PusherService &webSocketService,
	const char *mainChannel
);

Display display;
HTTPInterface interface(
	[](const char* message) {Serial.print(message);}
);
LockerService lockerService;

void setup()
{
	const char *mainChannel = "presence-locker-device";
	Serial.begin(115200);
	display.init();
	autoSubscribeToChannel(
		interface,
		webSocketService,
		mainChannel
	);
	registerHandlers(
		webSocketService,
		lockerService,
		&display,
		mainChannel
	);
	localNetwork.init();
}

uint8_t connectionAttempts = 0;

void loop()
{
	if (run) {
		if (webSocketService.poll()) {
			connectionAttempts = 0;
		} else {
			connectionAttempts++;
			display.displayText(
				String("Reestablishing connection... in "
					+ String(connectionAttempts) 
					+ " seconds").c_str());
			delay(1000 * connectionAttempts);
		}
	}
	lockerService.poll();
	delay(150);
}
