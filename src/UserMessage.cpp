#include "UserMessage.h"

UserMessage::UserMessage(const char *message) : message(message) {}

static const String extractor(const char *message, const char *key) {
	String value = "";
	int valueStart = String(message).indexOf(key) + strlen(key) + 1;
	int valueEnd = String(message).indexOf("\"", valueStart);
	value = String(message).substring(valueStart, valueEnd);
	return value;
}

const String UserMessage::bicycle_id() {
	return extractor(this->message, "bicycle_id");
}

const String UserMessage::username() {
	return extractor(this->message, "username");
}

const String UserMessage::lang() {
	return extractor(this->message, "lang");
}

const String UserMessage::info() {
	return extractor(this->message, "info");
}