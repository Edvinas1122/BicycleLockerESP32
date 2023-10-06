#include "WebSocketService.h"

WebSocketService::EventHandler::EventHandler(
	const char *eventKey,
	MessageCallback callback
): eventKey(eventKey), callback(callback), registered(true) {};

WebSocketService::EventHandler::EventHandler(
	const EventHandler &other
) {
	this->eventKey = other.eventKey;
	this->callback = other.callback;
	this->registered = other.registered;
}

WebSocketService::EventHandler &WebSocketService::EventHandler::operator=(const WebSocketService::EventHandler &other) {
	this->eventKey = other.eventKey;
	this->callback = other.callback;
	this->registered = other.registered;
	return *this;
}

void WebSocketService::EventHandler::execute(const String &message) {
	this->callback(message);
}

void WebSocketService::EventHandler::setCallback(MessageCallback callback) {
	this->callback = callback;
}

bool WebSocketService::EventHandler::isEnabled() const {
	return this->registered;
}