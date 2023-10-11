#include <freertos/FreeRTOS.h>
#include <Display.h>
#include <Network.h>
#include "env.h"
#include "LockerService.h"
#include <Pusher.h>
#include <HTTPFetch.h>


Display display;

bool run = false;
Network	localNetwork(
			LOCAL_SSID,
			LOCAL_USER,
			LOCAL_PASSWORD,
			[](const char* message) {Serial.print(message);},
			[]() {
				run = true;
				display.displayText("Have established connection.");
			},
			[]() {
				run = false;
				WiFi.disconnect(true);
				display.displayText("Not connected.");
			}
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
	Display *display,
	const char *mainChannel
);


HTTPInterface interface(
	[](const char* message) {Serial.print(message);}
);

/*
	A method after once the button is pressed in 
	a commenced sequence
*/
LockerService::LockerSequenceCallBack lockerSequenceCallback(
	PusherService& socket,
	HTTPInterface& lockerService,
	Display* display,
	const char* mainChannel
);

LockerService::LockerSequenceCallBack callback = lockerSequenceCallback(
	webSocketService,
	interface,
	&display,
	"presence-locker-device"
);

const char *mainChannel = "presence-locker-device";

LockerService lockerService(
	callback
);

void setup()
{
	Serial.begin(115200);
	display.init();
	lockerService.init();
	autoSubscribeToChannel(
		interface,
		webSocketService,
		&display,
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
				String("Pusher reconnecting... in "
					+ String(connectionAttempts) 
					+ " seconds").c_str());
			delay(1000 * connectionAttempts);
		}
	}
	lockerService.poll();
	delay(150);
}
