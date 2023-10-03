#ifndef PARSE_MESSAGE_H
# define PARSE_MESSAGE_H

#include <Arduino.h>

class UserMessage {
	public:
	UserMessage(const char *message);

	const String bicycle_id();
	const String username();
	const String lang();
	const String info();

	private:
	const char *message;
};

#endif
