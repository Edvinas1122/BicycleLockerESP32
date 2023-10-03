#include "Pusher.h"

PusherService::Message::Message(websockets::WebsocketsMessage message) {
    deserializeJson(json, message.data());
}

PusherService::Message::Message(const char *message) {
    deserializeJson(json, message);
}

const String PusherService::Message::event() {
    if (json.containsKey("event")) {
        return json["event"].as<String>();
    }
    return "";
}

const String PusherService::Message::message() {
    if (json.containsKey("data")) {
        return json["data"].as<String>();
    }
    return "";
}

const String PusherService::Message::getItem(const char *item) {
    if (json.containsKey(item)) {
        return json[item].as<String>();
    }
    return "";
}