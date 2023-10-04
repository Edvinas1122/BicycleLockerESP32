#include "WebSocketService.h"

WebSocketService::WebSocketService(
	const String &url
): url(url), connected(false) {
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
	Serial.print("Connecting to websocket... ");
	Serial.println(url);
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
			const String eventKey = WebSocketService::Message(msg).event();
			const String message = WebSocketService::Message(msg).message();
			useHandleEvent(eventKey.c_str(), message);
		} catch (const std::exception& e) {
			Serial.println("Exception caught: " + String(e.what()));
		}
	});
}
