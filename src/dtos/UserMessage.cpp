#include "dtos/UserMessage.h"

UserMessage::UserMessage(const char *message) : doc(1024) { // assuming max 1024 bytes for the JSON document
	deserializeJson(doc, message);
}

const String UserMessage::bicycle_id() {
    return doc["bicycle_id"].as<String>();
}

const String UserMessage::username() {
    return doc["username"].as<String>();
}

const String UserMessage::lang() {
    return doc["lang"].as<String>();
}

const String UserMessage::info() {
    return doc["info"].as<String>();
}