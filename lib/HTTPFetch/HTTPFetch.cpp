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
	HTTPClient &http,
	const char **data
) {
	http.begin(client, endpoint);
	for (int i = 0; headers[i] != nullptr; i++) {
		http.addHeader(headers[i], headers[++i]);
	}
	const int response_code = http.POST(getMessage(data));
	Serial.println(
		String("POST to server: " + String(response_code)).c_str()
	);
	const String response = http.getString();
	if (response_code != 200) {
		throw std::runtime_error(
			String("POST to server failed: " + response).c_str()
		);
	}	
	http.end();
	return response;
}