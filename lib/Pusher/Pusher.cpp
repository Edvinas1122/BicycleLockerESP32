#include "Pusher.h"

/*
	[scheme]://ws-[cluster_name].pusher.com:[port]/app/[key]
	establish a websocket connection to the cluster
*/ 
PusherService::PusherService(
	const char *key,
	const char *cluster
) {
	this->fullUrl = "ws://ws-";
	this->fullUrl += cluster;
	this->fullUrl += ".pusher.com:80/app/";
	this->fullUrl += key;
	this->fullUrl += "?client=js&version=7.0.3&protocol=5";
	// this->handleConnection();
	this->channel = NULL;
}

bool PusherService::poll() {
	const bool available = this->available();
	if (available) {
		websockets::WebsocketsClient::poll();
	} else {
		this->handleConnection();
	}
	return available;
}

void PusherService::subscribeToChannel(const char *channel) {
	const bool channelUnset = this->channel == NULL;
	this->channel = channel;
	if (channelUnset) {
		this->handleConnection();
	}
}

void PusherService::registerEventHandler(
	const char *eventKey,
	MessageCallback callback
) {
	this->messageHandlers[eventKey] = callback;
}

static String extractEventKeyFromMessage(websockets::WebsocketsMessage msg) {
	String eventKey = "";
	int eventKeyStart = msg.data().indexOf("\"event\":\"") + 9;
	int eventKeyEnd = msg.data().indexOf("\"", eventKeyStart);
	eventKey = msg.data().substring(eventKeyStart, eventKeyEnd);
	return eventKey;
}

void PusherService::handleSubscription() {
	String message = "{\"event\":\"pusher:subscribe\",\"data\":{\"channel\":\"";
	message += channel;
	message += "\"}}";
	this->send(message.c_str());
	this->onMessage([this](websockets::WebsocketsMessage msg){
		Serial.println("Got Message: " + msg.data());
		String eventKey = extractEventKeyFromMessage(msg);

		if(messageHandlers.count(std::string(eventKey.c_str()))) {
			messageHandlers[std::string(eventKey.c_str())](msg);
		} else {
			Serial.println("No handler registered for this event. eventKey: " + eventKey);
			Serial.println("All registered event keys:");
            for(const auto& pair : messageHandlers) {
                Serial.println(pair.first.c_str());  // Print the key
            }
		}
	});
}

void PusherService::handleConnection() {
	bool connected = this->connect(fullUrl);
	if (connected) {
		Serial.println("Connected!");
		if (this->channel != NULL) {
			this->handleSubscription();
		} else {
			Serial.println("No channel set!");
		}
	} else {
		Serial.println("Not Connected!");
	}
}

