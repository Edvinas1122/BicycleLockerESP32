#ifndef FETCH_H
# define FETCH_H

#include <Arduino.h>
#include <HTTPFetch.h>

/*
    Here we define all HTTP endpoints we going to interact in a runtime
    with data phrasing functions "lets readjust data"
*/
extern const char *endpoint;

constexpr char requestAuthorize[] = "authorize";

template <>
struct RequestMap<requestAuthorize> {
    static HTTPInterface::Request value;
};

extern const char* authReqHeaders[];

const String composeBicycleSocketGateAuthMessage(const char **data);

HTTPInterface::Request RequestMap<requestAuthorize>::value = HTTPInterface::Request(
    composeBicycleSocketGateAuthMessage,
	authReqHeaders,
    endpoint
);

#endif