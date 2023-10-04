#include "Locker.h"
#include <WiFiClientSecure.h>
#include <Pusher.h>
#include "fetch.hpp"

const char *mainChannel = "presence-locker-device";

void registerHandlers(
	PusherService &lockerService,
	LockerService &lockerInterfaceService
) {
	lockerService.registerEventHandler(
		"pusher_internal:subscription_succeeded",
		[](String& message) {
			Serial.print("Subscription succeeded!");
	});
	lockerService.registerEventHandler(
		"pusher_internal:member_added",
		[&lockerInterfaceService](String& message) {
			Serial.print("Member added!");
			Serial.println(message);
			PusherService::Message msg(message.c_str());
			PusherService::Message user(msg.getItem("user_info").c_str());
			lockerInterfaceService.addName(user.getItem("name").c_str());
	});
	lockerService.registerEventHandler(
		"pusher_internal:member_removed",
		[&lockerInterfaceService](String& message) {
			Serial.print("Member removed!");
			Serial.println(message);
			PusherService::Message msg(message.c_str());
			PusherService::Message user(msg.getItem("user_info").c_str());
			lockerInterfaceService.removeName(user.getItem("name").c_str());
	});
	lockerService.registerEventHandler(
		"ping",
		[](String& message) {
			Serial.print("Ping!");
			Serial.println(message);
	});
	lockerService.registerEventHandler(
		"pusher:error",
		[](String &message) {
			Serial.println("pusher:error Triggered");
			Serial.println(message);
		}
	);
	lockerService.registerEventHandler(
		"client-ping",
		[&lockerService](String& message) {
			Serial.println(message);
			lockerService.sendMessage("client-pong",
				mainChannel,
				"{\"message\":\"online\"}"
			);
	});
}

/*
	https://pusher.com/docs/channels/library_auth_reference/pusher-websockets-protocol/#connection-closure
*/
void handleNetworkTasks(
	bool *run,
	Display *display
) {
	LockerService lockerInterfaceService(
		display
	);
	PusherService lockerOnlineService(
		PUSHER_KEY,
		PUSHER_CLUSTER
	);
	WiFiClientSecure client;
	HTTPClient http;

	client.setInsecure();
	lockerOnlineService.subscribeToChannel(
		mainChannel,
		[&client, &http](const String& socket_id) -> String {
		Serial.println("Subscribing to presence-locker-device");
		String response = postToAuthPusherEndpoint(client, http,
							socket_id.c_str(), "presence-locker-device");
		Serial.println(response);
		return response;
	});
	registerHandlers(
			lockerOnlineService,
			lockerInterfaceService
	);

	int connectionAttempts = 0;
	while (*run) {
		if (lockerOnlineService.poll()) {
			connectionAttempts = 0;
		} else {
			connectionAttempts++;
			display->displayText(
				String("Reestablishing connection... in "
					+ String(connectionAttempts) 
					+ " seconds").c_str());
			delay(1000 * connectionAttempts);
		}
		delay(200);
	}
}