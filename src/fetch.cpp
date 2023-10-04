#include "fetch.hpp"

const char *endpoint = "https://bicycle-share-front-end-git-deviceinterface-edvinas1122.vercel.app/pusher/auth";

void postToPusherEndpoint(
	WiFiClientSecure &client,
	HTTPClient &http,
	const char *message,
	const char *event
) {
	http.begin(client, "https://bicycle-share-front-end.vercel.app/pusher/update");
	http.addHeader("Content-Type", "application/json");
	http.addHeader("x-password", PUSHER_KEY);
	String body = "{\"event\":\"";
	body += event;
	body += "\",\"message\":\"";
	body += message;
	body += "\"}";
	const int response_code = http.POST(body);
	Serial.println(
		String("POST to server: " + String(response_code)).c_str()
	);
	http.end();
}

String postToAuthPusherEndpoint(
	WiFiClientSecure &client,
	HTTPClient &http,
	const char *socket_id,
	const char *channel_name
) {
	Serial.println("Posting to auth endpoint");
	Serial.println(socket_id);
	Serial.println(channel_name);

	http.begin(client, endpoint);
	http.addHeader("Content-Type", "application/json");
	http.addHeader("x-password", PUSHER_KEY);
	String body;
	body.reserve(50);
	body += "{\"socket_id\":\"";
	body += socket_id;
	body += "\",\"channel_name\":\"";
	body += channel_name;
	body += "\"}";
	const int response_code = http.POST(body);
	if (response_code != 200) {
		throw std::runtime_error("Failed to authenticate");
	}
	Serial.println(
		String("POST to server: " + String(response_code)).c_str()
	);
	String responseBody = http.getString(); // Get the response body
	http.end();
	return responseBody; // Return the response body
}
