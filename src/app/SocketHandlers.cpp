#include <Display.h>
#include <Pusher.h>
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
	});
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
		},
		mainChannel,
		[lockerDisplay](const char *id)->bool {
			return true;
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
			Serial.print("Register open commit: ");
			Serial.println(msg.getItem("message"));
			bool commenced = device.commitOpenSequence(
				atoi(msg.getItem("bicycle_id").c_str()),
				msg.getItem("message"),
				last, // is in global scope
				msg.getItem("duration")
			);
			Serial.print("commenced: ");
			Serial.println(commenced);
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
			// sequenceTimerDisplay(lockerDisplay);
		},
		mainChannel,
		[device](const char *id)->bool {
			last = id; // is in global scope
			return !device.inCommitedOpenSequence();
		}
	);
	webSocketService.registerEventHandler(
		"lend-status",
		[&device](const String &message){
			device.endOpenSequence();
		},
		mainChannel
	);
	webSocketService.registerEventHandler(
		"client-sequence-abort",
		[&device, lockerDisplay](const String &message){
			device.endOpenSequence();
			lockerDisplay->message("Unlock aborted", 3);
		},
		mainChannel,
		[&device](const char *id)->bool {
			return device.requestCancelPermit(id);
		}
	);
}


/*
	https://pusher.com/docs/channels/server_api/authorizing-users/
	subs to presence channel as Pusher only allows sending messages on
	presence and private channels.
	HTTPInterface is a dependency for an action because,
	we need to make a request to our web service to get authourization.
*/
void autoSubscribeToChannel(
	HTTPInterface &fetcher,
	PusherService &lockerOnlineService,
	Display *display,
	const char *mainChannel
) {
	lockerOnlineService.registerEventHandler(
		connectionEvent,
		[&fetcher, &lockerOnlineService, mainChannel, display](
			const String& message
		) {
			lockerOnlineService.socket_id
				= WebSocketService::Message(message.c_str())
					.getItem("socket_id");
			const char* data[] = {lockerOnlineService.socket_id.c_str(),
									mainChannel, NULL };
			const String response = fetcher.post<requestAuthorize>(data);
			lockerOnlineService.Subscribe(mainChannel, response);
			display->message("Pusher Service Connected", 3000);
	});
}


const String SequenceReport(const bool unlocked) {
	return String("{\"message\":") + (unlocked ? "\"unlocked\"" : "\"timedout\"") + "}";
}

const String SequenceReportDisplay(const bool unlocked) {
	return String("Locker ") + (unlocked ? "unlocked" : "timedout");
}

/*
	A callback for when an unlock sequence is completed.
	// provide api:
		"user_id",
		"bicycle_id" = "locker_id",
		"duration"
*/

#define PURPOSE_OPEN "open"
#define PURPOSE_RETURN "return"
#define PURPOSE_BORROW "give"

LockerService::LockerSequenceCallBack lockerSequenceCallback(
	PusherService& socket,
	HTTPInterface& fetcher,
	Display* display,
	const char* mainChannel
) {
	return [&socket, &fetcher, mainChannel, display](
		const bool unlocked,
		const String& requestee,
		const String& purpose,
		const String& lockerId,
		const String& duration
	) {
		display->message("Please wait...", 2000); // make loading interval
		socket.sendMessage(
			"client-locker-button-press",
			mainChannel,
			"{\"message\":\"btn-prs\"}"
		);
		if (unlocked) {
			if (purpose == PURPOSE_BORROW) {
				const char* data[] = {
					"borrow",
					requestee.c_str(),
					lockerId.c_str(),
					duration.c_str(),
					NULL
				};
				fetcher.post<updateDatabase>(data);
			} else if (purpose == PURPOSE_RETURN) {
				const char* data[] = {
					"return",
					requestee.c_str(),
					lockerId.c_str(),
					NULL
				};
				fetcher.post<updateDatabase>(data);
			}
		}
		socket.sendMessage(
			"client-locker-register",
			mainChannel,
			SequenceReport(unlocked).c_str()
		);
		display->message(
			SequenceReportDisplay(unlocked).c_str(),
			3
		);
	};
}
