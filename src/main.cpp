#include <freertos/FreeRTOS.h>
#include <Display.h>
#include <Network.h>
#include "env.h"
#include "Locker.h"
#include <Pusher.h>
#include <HTTPFetch.h>


bool run = false;

Network	localNetwork(
			LOCAL_SSID,
			LOCAL_USER,
			LOCAL_PASSWORD,
			[](const char* message) {Serial.print(message);},
			[]() {run = true;},
			[]() {run = false;}
		);


PusherService lockerOnlineService(
	PUSHER_KEY,
	PUSHER_CLUSTER,
	[](const char* message) {Serial.print(message);}
);

void registerHandlers(
	PusherService &lockerOnlineService,
	Display *display,
	const char *mainChannel
);
void autoSubscribeToChannel(
	HTTPInterface &interface,
	PusherService &lockerOnlineService,
	const char *mainChannel
);

Display display;
HTTPInterface interface(
	[](const char* message) {Serial.print(message);}
);

void setup()
{
	const char *mainChannel = "presence-locker-device";
	Serial.begin(115200);
	display.init();
	autoSubscribeToChannel(
		interface,
		lockerOnlineService,
		mainChannel
	);
	registerHandlers(
		lockerOnlineService,
		&display,
		mainChannel
	);
	run = false;
	localNetwork.init();
}

uint8_t connectionAttempts = 0;

void loop()
{
	if (run) {
		if (lockerOnlineService.poll()) {
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
	delay(200);
}
