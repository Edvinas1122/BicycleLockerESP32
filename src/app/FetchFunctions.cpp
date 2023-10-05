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
	body += data[0];  // First item
	body += "\",\"channel_name\":\"";
	body += data[1];  // Second item
	body += "\"}";
	return body;
}