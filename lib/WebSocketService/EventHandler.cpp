#include "WebSocketService.h"

// WebSocketService::EventHandler::EventHandler(
// 	const char *eventKey,
// 	MessageCallback callback
// ) {
// 	this->eventKey = eventKey;
// 	this->callback = callback;
// 	this->registered = true;
// }

// WebSocketService::EventHandler::~EventHandler() {
// 	// this->pusher->unregisterEventHandler(this->eventKey);
// }

void WebSocketService::EventHandler::execute(const String &message) {
	this->callback(message);
}