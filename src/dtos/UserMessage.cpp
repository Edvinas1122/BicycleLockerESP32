#include "dtos/UserMessage.h"

UserMessage::UserMessage(const char *message) {
	deserializeJson(doc, message);
}

const String UserMessage::bicycle_id() const {
    return doc["bicycle_id"].as<String>();
}

const String UserMessage::username() const {
    return doc["username"].as<String>();
}

const String UserMessage::lang() const {
    return doc["lang"].as<String>();
}

const String UserMessage::info() const {
    return doc["info"].as<String>();
}