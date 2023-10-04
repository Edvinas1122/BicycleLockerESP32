#include "Pusher.h"

PusherService::EventHandler::EventHandler(
	PusherService *pusher,
	const char *eventKey,
	MessageCallback callback
) {
	this->pusher = pusher;
	this->eventKey = eventKey;
	this->callback = callback;
	this->registered = true;
}

// PusherService::EventHandler::~EventHandler() {
// 	this->pusher->unregisterEventHandler(this->eventKey);
// }

void PusherService::EventHandler::execute(const String &message) {
	this->callback(message);
}