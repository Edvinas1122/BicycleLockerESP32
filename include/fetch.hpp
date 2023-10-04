#ifndef FETCH_H
# define FETCH_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

void postToPusherEndpoint(
	WiFiClientSecure &client,
	HTTPClient &http,
	const char *message,
	const char *event = "ping"
);

String postToAuthPusherEndpoint(
	WiFiClientSecure &client,
	HTTPClient &http,
	const char *socket_id,
	const char *channel_name = "ping"
);


#endif