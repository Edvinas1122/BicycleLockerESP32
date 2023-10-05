#include "WebSocketService.h"

WebSocketService::WebSocketService(
	const String &url,
	void (*log)(const char *) /*= defaultLog*/
): url(url), connected(false), log(log) {
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
	log("Connecting to "); log(url.c_str()); log("...\n");
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
			log("Event: "); log(eventKey.c_str()); log(" Message: "); log(message.c_str()); log("\n");
 			useHandleEvent(eventKey.c_str(), message);
		} catch (const std::exception& e) {
			log("Exception caught: "); log(e.what()); log("\n");
			connected = false;
		}
	});
}

void WebSocketService::useHandleEvent(const char *eventKey, const String &message) {
	EventHandlerIterator it = eventHandlerMap.find(std::string(eventKey));
	if (it != eventHandlerMap.end()) {
		try {
			it->second.execute(message);
		} catch (const std::exception &e) {
			Serial.println("Exception: " + String(e.what()));
			connected = false;
		}
	} else {
		Serial.println("Unknown event: " + String(eventKey));
	}
}

void WebSocketService::defaultLog(const char *message) {
	(void)message;
}