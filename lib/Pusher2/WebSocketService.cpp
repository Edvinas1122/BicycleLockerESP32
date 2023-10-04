#include "Pusher.h"

WebSocketService::WebSocketService(
	const char *url
): url(url) {
	this->connected = false;
	this->setupEventDriver();
}

WebSocketService::~WebSocketService() {
	this->close();
	this->connected = false;
};

bool WebSocketService::poll() {
	connected = this->available();
	if (connected) {
		websockets::WebsocketsClient::poll();
	} else {
		this->handleConnection();
	}
	return connected;
}

void WebSocketService::handleConnection() {
	connected = this->connect(url);
	if (connected) {
		Serial.println("Connected!");
	} else {
		Serial.println("Not Connected!");
	}
}

void WebSocketService::setupEventDriver() {
	this->onMessage([this](websockets::WebsocketsMessage msg){
		try {
			String eventKey = WebSocketService::Message(msg).event();
			if(hasEventHandler(eventKey.c_str())) {
				String message = WebSocketService::Message(msg).message();
				handleEvent(eventKey.c_str(), message);
			} else {
				Serial.println("No handler registered for this event. eventKey: " + eventKey);
			}
		} catch (const std::exception& e) {
			Serial.println("Exception caught: " + String(e.what()));
		}
	});
}