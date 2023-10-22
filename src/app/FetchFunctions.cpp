#include "FetchFunctions.hpp"
#include "env.h"

const char *endpoint = BICYCLE_API_ENDPOINT;

const char* authReqHeaders[] = {
	"Content-Type:",
	"application/json",
	"x-password",
	PUSHER_KEY,
	nullptr  // Mark the end of your headers
};

/*
	In case of methods, stateful extraction, or in general
	better to define seperate for better decaupling practices.
*/

/*
	https://pusher.com/docs/channels/server_api/authorizing-users/#implementing-the-authorization-endpoint-for-a-private-channel
	once gets from a socket its own id upon connection, provides
	our web service data.
*/
const String composeBicycleSocketGateAuthMessage(const char **data){
	String body;
	body.reserve(40);
	body += "{\"socket_id\":\"";
	body += data[0];
	body += "\",\"channel_name\":\"";
	body += data[1];
	body += "\"}";
	return body;
}

const char *endpoint_db = UPDATE_DB_ENDPOINT;

/*
	// provide api:
		"user_id",
		"bicycle_id" = "locker_id",
		"duration"
*/
const String composeRegisterUserToDatabaseMessage(const char **data){
    String body;
    body.reserve(50);
    body += "{\"purpose\":\"";
    body += data[0];
    body += "\",\"user_id\":";
    body += data[1];
    body += ",\"bicycle_id\":";
    body += data[2];
    if (data[3] != NULL) {
        body += ",\"duration\":\"";
        body += data[3];
        body += "\"";
    }
    body += "}";
    return body;
}