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
	this->connected = false;
	this->setupEventDriver();
}

void PusherService::registerEventHandler(
	const char *eventKey,
	MessageCallback callback
) {
	this->messageHandlers[eventKey] = callback;
}

bool PusherService::poll() {
	connected = this->available();
	if (connected) {
		websockets::WebsocketsClient::poll();
	} else {
		this->handleConnection();
	}
	return connected;
}

void PusherService::subscribeToChannel(const char *channel) {
	if (connected) {
		this->sendSubReq(channel);
	} else {
		this->registerEventHandler("pusher:connection_established", [this, channel](String& message){
			this->sendSubReq(channel);
		});
	}
}

void PusherService::sendSubReq(const char *channel) {
	String message = "{\"event\":\"pusher:subscribe\",\"data\":{\"channel\":\"";
	message += channel;
	message += "\"}}";
	this->send(message.c_str());
}

void PusherService::setupEventDriver() {
	this->onMessage([this](websockets::WebsocketsMessage msg){
		try {
			String eventKey = PusherService::Message(msg).event();
			if(messageHandlers.count(std::string(eventKey.c_str()))) {
				String message = PusherService::Message(msg).message();
				messageHandlers[std::string(eventKey.c_str())](message);
			} else {
				Serial.println("No handler registered for this event. eventKey: " + eventKey);
			}
		} catch (const std::exception& e) {
			Serial.println("Exception caught: " + String(e.what()));
		}
	});
}

const bool PusherService::handleConnection() {
	connected = this->connect(fullUrl);
	if (connected) {
		Serial.println("Connected!");
	} else {
		Serial.println("Not Connected!");
	}
	return connected;
}
