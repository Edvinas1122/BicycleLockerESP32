#include "HTTPFetch.h"

HTTPInterface::Request::Request(
	const String (*getMessage)(char const **data),
	const char **headers,
	const char *endpoint
):
	getMessage(getMessage),
	headers(headers),
	endpoint(endpoint) {}

const String HTTPInterface::Request::execute(
	WiFiClientSecure &client,
	const char **data,
	void (*log)(const char *)
) {
	HTTPClient http;
	http.begin(client, endpoint);
	for (int i = 0; headers[i] != nullptr; i++) {
		http.addHeader(headers[i], headers[++i]);
	}
	Serial.println(getMessage(data));
	Serial.println(endpoint);
	Serial.println(headers[0]);
	Serial.println(headers[1]);
	const uint8_t response_code = http.POST(getMessage(data));
	log("POST request status:"); log(String(response_code).c_str());
	if (response_code == 144) {
		throw std::runtime_error(
			String("POST to server failed: "
				+ http.errorToString(response_code)).c_str()
		);
	}
	const String response = http.getString();
	if (response_code != 200) {
		throw std::runtime_error(
			String("POST to server failed: "
				+ response).c_str()
		);
	}
	Serial.println(response);
	http.end();
	return response;
}