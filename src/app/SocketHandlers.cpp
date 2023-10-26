#include <Display.h>
#include <Pusher.h>
#include <Cypher.h>
#include "FetchFunctions.hpp"
#include "LockerService.h"

constexpr char openLockerEvent[] = "client-open-locker";
constexpr char questionLockerAvailabilityEvent[] = "client-question-locker-availability";

static const String getSequenceMessage(bool available) {
	return String(available ? "\"begin\"" : "\"false\"");
}

const String actionDisplay(const String &requestee, const char *action) {
	return String(requestee + action);
}


/*
	Resolves scope problem
	passes user id between callbacks
*/
String last;

void registerHandlers(
	Cypher *cypher,
	PusherService &webSocketService,
	LockerService &device,
	Display *lockerDisplay,
	const char *mainChannel
) {
	webSocketService.registerEventHandler(
		subscriptionSucceededEvent,
		[lockerDisplay](const String& message) {
		lockerDisplay->const_message("Online");
	});
	webSocketService.registerEventHandler(
		memberAddedEvent,
		[lockerDisplay](const String& message) {
			PusherService::Message msg(message.c_str());
			PusherService::Message user(msg.getItem("user_info").c_str());
			lockerDisplay
				->message(
					actionDisplay(
						user.getItem("name").c_str(),
						" is present"
					).c_str(),
					3
				);
	});
	webSocketService.registerEventHandler(
		memberRemovedEvent,
		[lockerDisplay](const String& message) {
			PusherService::Message msg(message.c_str());
			PusherService::Message user(msg.getItem("user_info").c_str());
			lockerDisplay
				->message(
					actionDisplay(
						user.getItem("name").c_str(),
						" has left"
					).c_str(),
					3
				);
		}
	);
	webSocketService.registerEventHandler(
		"pusher:ping",
		[&webSocketService](const String& message) {
			webSocketService.sendMessage("pusher:pong", "", "{}");
		}
	);
	webSocketService.registerEventHandler(
		"client-ping",
		[&webSocketService, mainChannel](const String& message) {
			webSocketService.sendMessage("client-pong",
				mainChannel,
				"{\"message\":\"online\"}"
			);
		}
	);
	webSocketService.registerEventHandler(
		openLockerEvent,
		[&webSocketService, &device, mainChannel, lockerDisplay](
			const String& message
		) {
			PusherService::Message msg(message.c_str());
			const String event = String(
				"client-open-seq-" + last // passed by global scope
			);
			bool commenced = device.registerRequest(
				msg.getItem("locker_id"),
				last // is in global scope
			);
			webSocketService.sendMessage(
				event.c_str(),
				mainChannel,
				getSequenceMessage(commenced).c_str()
			);
			DisplayMods::countdownTimer(
				lockerDisplay,
				"Press button to open locker: ",
				10
			);
		},
		mainChannel,
		[device](const char *id)->bool {
			last = id; // is in global scope
			return device.available();
		}
	);
	// webSocketService.registerEventHandler(
	// 	"lend-status",
	// 	[&device](const String &message){
	// 		device.
	// 	},
	// 	mainChannel
	// );
	webSocketService.registerEventHandler(
		"client-sequence-abort",
		[&device, lockerDisplay](const String &message){
			lockerDisplay->message("Unlock aborted", 3);
			device.cancelRequest(last);
		},
		mainChannel,
		[&device](const char *id)->bool {
			last = id;
			return !device.available();
		}
	);
	webSocketService.registerEventHandler(
		"unlock-command",
		[&device, lockerDisplay](const String &message){
			PusherService::Message msg(message.c_str());
			const String locker = msg.getItem("locker_id");
			lockerDisplay->message("Unlocked", 5);
			lockerDisplay->sequence_message("Close locker", 2);
			device.unlock(locker);
		}
	);
	// webSocketService.registerEventHandler(
	// 	"test-rsa",
	// 	[cypher](const String &message){
	// 		PusherService::Message msg(message.c_str());
	// 		String received = msg.getItem("message");
	// 		const char *signature = msg.getItem("signature").c_str();
	// 		const bool valid = cypher->verify(
	// 			received.c_str(),
	// 			signature
	// 		);
	// 	}
	// );
}


/*
	https://pusher.com/docs/channels/server_api/authorizing-users/
	subs to presence channel as Pusher only allows sending messages on
	presence and private channels.
	HTTPInterface is a dependency for an action because,
	we need to make a request to our web service to get authourization.
*/
static void subscribeToPresenceChannel(
	HTTPInterface &fetcher,
	PusherService &socket,
	const char *channel
) {
	const char* data[] = {socket.socket_id.c_str(), channel, NULL };
	const String response = fetcher.post<requestAuthorize>(data);
	socket.Subscribe(channel, response);
}

void autoSubscribeToChannel(
	HTTPInterface &fetcher,
	PusherService &socket,
	Display *display,
	const char *mainChannel
) {
	socket.registerEventHandler(
		connectionEvent,
		[&fetcher, &socket, mainChannel, display](
			const String& message
		) {
			socket.socket_id
				= WebSocketService::Message(message.c_str())
					.getItem("socket_id");
			subscribeToPresenceChannel(
				fetcher,
				socket,
				mainChannel
			);
			display->message("Pusher Service Connected", 3000);
	});
}
