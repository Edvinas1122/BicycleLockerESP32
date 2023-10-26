#ifndef EVENT_HANDLING_H
# define EVENT_HANDLING_H

# include <LockerService.h>
# include <Cypher.h>
# include <utils.h>
# include <ArduinoJson.h>

LockerService::OpenRequest
	::SignMethod getSignatureMethod(
		Cypher *cypher
	);

# include <Pusher.h>
# include <Display.h>

LockerService
	::ButtonPressCallBack 
		getButtonPressCallback(
			PusherService& socket,
			Display* display,
			const char* mainChannel
		);

# include <HTTPFetch.h>

void registerHandlers(
	Cypher *cypher,
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

#endif