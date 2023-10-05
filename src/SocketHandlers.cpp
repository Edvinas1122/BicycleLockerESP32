#include <Display.h>
#include <Pusher.h>
#include "FetchFunctions.hpp"

void registerHandlers(
	PusherService &lockerService,
	Display *lockerDisplay,
	const char *mainChannel
) {
	lockerService.registerEventHandler(
		subscriptionSucceededEvent,
		[lockerDisplay](const String& message) {
		Serial.println("subscriptionSucceededEvent");
		lockerDisplay->displayText("Online");
	});
	lockerService.registerEventHandler(
		memberAddedEvent,
		[lockerDisplay](const String& message) {
			PusherService::Message msg(message.c_str());
			PusherService::Message user(msg.getItem("user_info").c_str());
			lockerDisplay->displayText(user.getItem("name").c_str());
	});
	lockerService.registerEventHandler(
		memberRemovedEvent,
		[lockerDisplay](const String& message) {
			lockerDisplay->clear();
	});
	lockerService.registerEventHandler(
		"client-ping",
		[&lockerService, mainChannel](const String& message) {
			lockerService.sendMessage("client-pong",
				mainChannel,
				"{\"message\":\"online\"}"
			);
	});
	lockerService.registerEventHandler(
		"client-",
		[&lockerService, mainChannel](const String& message) {
			lockerService.sendMessage("client-pong",
				mainChannel,
				"{\"message\":\"online\"}"
			);
	});
}


/*
	https://pusher.com/docs/channels/server_api/authorizing-users/
	subs to presence channel as Pusher only allows sending messages on
	presence and private channels.
	HTTPInterface is a dependency for an action because,
	we need to make a request to our web service to get authourization.
*/
void autoSubscribeToChannel(
	HTTPInterface &interface,
	PusherService &lockerOnlineService,
	const char *mainChannel
) {
	lockerOnlineService.registerEventHandler(
		connectionEvent,
		[&interface, &lockerOnlineService, mainChannel](const String& message) {
			lockerOnlineService.socket_id = WebSocketService::Message(message.c_str()).getItem("socket_id");
			const char* data[] = {lockerOnlineService.socket_id.c_str(), mainChannel, NULL};
			const String response = interface.post<requestAuthorize>(data);
			lockerOnlineService.Subscribe(mainChannel, response);
	});
}
