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
	this->socket_id.reserve(20); // short string
	this->setupEventDriver();
}

PusherService::~PusherService() {
	this->close();
	this->connected = false;
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


void PusherService::subscribeToChannel(
	const char *channel,
	std::function<String(const String&)> authCallback
) {
	if (connected) {
		this->sendSubReq(channel, authCallback(socket_id));
	} else {
		this->registerEventHandler(
			"pusher:connection_established",
			[this, channel, authCallback](const String& message) {
				Serial.println("Connection established, subscribing to channel: " + message);
				socket_id = PusherService::Message(message.c_str()).getItem("socket_id");
				Serial.println("socket_id: " + socket_id);
				try {
					this->sendSubReq(channel, authCallback(socket_id));
				} catch (const std::exception& e) {
					Serial.println("Exception caught: " + String(e.what()));
				}
			}
		);
	}
}

static const String generateSubMessage(
	const char *channel
) {
    String message;
	message.reserve(100);
	message += "{\"event\":\"pusher:subscribe\",\"data\":{\"channel\":\"";
    message += channel;
    message += "\"}}";
    return message;
}

static const String generateSubMessagePresenceChannel(
	const char *channel,
	const String &data
) {
	String message;
	message.reserve(100);
	message += "{\"event\":\"pusher:subscribe\",\"data\":{\"channel\":\"";
	message += channel;
	message += "\",";
	message += data.substring(1);
	message += "}";
	return message;
}

void PusherService::sendSubReq(const char *channel, const String &signature) {
	if (signature.length() > 0) {
		const String mesage = generateSubMessagePresenceChannel(channel, signature).c_str();
		Serial.println("Sending message: " + mesage);
		this->send(mesage);
	}else {
    	this->send(generateSubMessage(channel).c_str());
	}
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

static const String formMessage(
	const char *event,
	const char *channel,
	const char *data
) {
	String message;
	message.reserve(100);
	message += "{\"event\":\"";
	message += event;
	message += "\",\"channel\":\"";
	message += channel;
	message += "\",\"data\":";
	message += data;
	message += "}";
	return message;
}

void PusherService::sendMessage(
	const char *event,
	const char *channel,
	const char *data
) {
	if (connected) {
		const String message = formMessage(event, channel, data);
		Serial.println("Sending message: " + message);
		this->send(message.c_str());
	} else {
		Serial.println("Not connected, can't send message");
	}
}
