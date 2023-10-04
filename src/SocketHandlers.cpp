#include <Display.h>
#include <Pusher.h>
#include "FetchFunctions.hpp"

void registerHandlers(
	PusherService &lockerService,
	Display *lockerDisplay,
	const char *mainChannel
) {
	lockerService.registerEventHandler(
		"pusher_internal:member_added",
		[lockerDisplay, mainChannel](const String& message) {
			Serial.print("Member added!");
			Serial.println(message);
			PusherService::Message msg(message.c_str());
			PusherService::Message user(msg.getItem("user_info").c_str());
			lockerDisplay->displayText(user.getItem("name").c_str());
	});
	lockerService.registerEventHandler(
		"pusher_internal:member_removed",
		[lockerDisplay, mainChannel](const String& message) {
			Serial.print("Member removed!");
			Serial.println(message);
			lockerDisplay->clear();
	});
	lockerService.registerEventHandler(
		"client-ping",
		[&lockerService, mainChannel](const String& message) {
			Serial.println(message);
			lockerService.sendMessage("client-pong",
				mainChannel,
				"{\"message\":\"online\"}"
			);
	});
}

void autoSubscribeToChannel(
	HTTPInterface &interface,
	PusherService &lockerOnlineService,
	const char *mainChannel
) {
	lockerOnlineService.registerEventHandler(
		connectionEvent,
		[&interface, &lockerOnlineService, mainChannel](const String& message) {
			Serial.println("Connection event!");
			Serial.println(message);
			lockerOnlineService.socket_id = WebSocketService::Message(message.c_str()).getItem("socket_id");
			const char *channel = "presence-locker-device";
			const char* data[] = {lockerOnlineService.socket_id.c_str(), channel, NULL};
			const String response = interface.post<requestAuthorize>(data);
			Serial.println(response);
			lockerOnlineService.Subscribe(mainChannel, response);
	});
}
