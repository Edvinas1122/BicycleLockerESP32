#ifndef PARSE_MESSAGE_H
# define PARSE_MESSAGE_H

#include <Arduino.h>
#include <ArduinoJson.h>

class UserMessage {
    public:
        UserMessage(const char *message);
        const String bicycle_id() const;
        const String username() const;
        const String lang() const;
        const String info() const;
    private:
        StaticJsonDocument<256> doc;
};

#endif