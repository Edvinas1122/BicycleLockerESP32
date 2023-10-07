#include <Display.h>
#include <Pusher.h>
#include "FetchFunctions.hpp"
#include "LockerService.h"

constexpr char openLockerEvent[] = "client-open-locker";
constexpr char questionLockerAvailabilityEvent[] = "client-question-locker-availability";

static const String getLockerMessage(bool available) {
	return String("{\"message\":") + (available ? "begin" : "false") + "}";
}

void registerHandlers(
	PusherService &webSocketService,
	LockerService &lockerService,
	Display *lockerDisplay,
	const char *mainChannel
) {
	webSocketService.registerEventHandler(
		subscriptionSucceededEvent,
		[lockerDisplay](const String& message) {
		lockerDisplay->displayText("Online");
	});
	webSocketService.registerEventHandler(
		memberAddedEvent,
		[lockerDisplay](const String& message) {
			PusherService::Message msg(message.c_str());
			PusherService::Message user(msg.getItem("user_info").c_str());
			lockerDisplay->displayText(user.getItem("name").c_str());
	});
	webSocketService.registerEventHandler(
		memberRemovedEvent,
		[lockerDisplay](const String& message) {
			lockerDisplay->clear();
	});
	webSocketService.registerEventHandler(
		"client-ping",
		[&webSocketService, mainChannel](const String& message) {
			webSocketService.sendMessage("client-pong",
				mainChannel,
				"{\"message\":\"online\"}"
			);
	});
	webSocketService.registerEventHandler(
		openLockerEvent,
		[&webSocketService, &lockerService, mainChannel](const String& message) {
			PusherService::Message msg(message.c_str());
			bool commenced = lockerService.commitOpenSequence(
				atoi(msg.getItem("locker_id").c_str()),
				""
			);
			webSocketService.sendMessage(
				"client-open-locker",
				mainChannel,
				getLockerMessage(commenced).c_str()
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
			lockerOnlineService.socket_id = WebSocketService::Message(message.c_str())
																	.getItem("socket_id");
			const char* data[] = {lockerOnlineService.socket_id.c_str(),
									mainChannel, NULL };
			const String response = interface.post<requestAuthorize>(data);
			lockerOnlineService.Subscribe(mainChannel, response);
	});
}

/*
	A callback for when an unlock sequence is completed.
*/
std::function<void(bool)> lockerSequenceCallback(
	PusherService& socket,
	HTTPInterface& fetcher,
	const char* mainChannel
) {
	return [&socket, &fetcher, mainChannel](bool unlocked) {
		socket.sendMessage(
			openLockerEvent,
			mainChannel,
			getLockerMessage(unlocked).c_str()
		);
		// fetcher.post<requestLockerStatus>();
	};
}