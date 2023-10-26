#include <freertos/FreeRTOS.h>
#include <Display.h>
#include <Network.h>
#include "LockerService.h"
#include <Cypher.h>
#include <utils.h>
#include "env.h"

Display display;

bool connected = false;
Network	localNetwork(
			LOCAL_SSID,
			LOCAL_USER,
			LOCAL_PASSWORD,
			[](const char* message) {Serial.print(message);},
			[]() {
				connected = true;
				fetchLocalTime();
				display.message("Have established connection.", 3);
			},
			[]() {
				connected = false;
				WiFi.disconnect(true);
				display.const_message("Not connected.");
			}
		);

#include <Pusher.h>

PusherService webSocketService(
	PUSHER_KEY,
	PUSHER_CLUSTER,
	[](const char* message) {Serial.print(message);}
);

#include "cypher_keys.h"

Cypher cypher(
	CYPHER_PUBLIC_KEY,
	CYPHER_PRIVATE_KEY
);

#include <HTTPFetch.h>

HTTPInterface interface(
	[](const char* message) {Serial.print(message);}
);

/*
	A method after once the button is pressed in 
	a commenced sequence
*/

const char *mainChannel = "presence-locker-device";

#include <EventHandling.h>

LockerService lockerService(
	getButtonPressCallback(
		webSocketService,
		&display,
		mainChannel),
	getSignatureMethod(&cypher)
);

String clockDisplay() {
	return getLocalTime();
}

void setup()
{
	Serial.begin(115200);
	display.init();
	DisplayMods::loadingDots(&display, "Connecting");
	cypher.init();
	lockerService.init();
	autoSubscribeToChannel(
		interface,
		webSocketService,
		&display,
		mainChannel
	);
	registerHandlers(
		&cypher,
		webSocketService,
		lockerService,
		&display,
		mainChannel
	);
	localNetwork.init();
	display.setMessageSource(clockDisplay);
}

uint8_t connectionAttempts = 0;
int64_t setReconnectTime = 0;

static bool shouldReconnect() {
	return setReconnectTime == 0 && xx_time_get_time() > setReconnectTime;
}

void loop()
{
	if (connected && shouldReconnect()) {
		if (webSocketService.poll()) {
			connectionAttempts = 0;
			setReconnectTime = 0;
		} else {
			connectionAttempts++;
			DisplayMods::countdownTimer(
				&display,
				"Pusher reconnecting... in ",
				connectionAttempts
			);
			setReconnectTime = xx_time_get_time() + 1000 * connectionAttempts;
			// delay(1000 * connectionAttempts);
		}
	}
	lockerService.poll();
	display.poll();
	delay(100);
}
