#ifndef HTTPFETCH_H
# define HTTPFETCH_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

template <const char* Key>
struct RequestMap;


class HTTPInterface {
public:
    class Request;

public:
    HTTPInterface(): 
        client(), http() {
            client.setInsecure();
    };

    virtual ~HTTPInterface() {};

	template<const char* RequestName>
	const String post(const char **data) {
		return RequestMap<RequestName>::value.execute(client, http, data);
	};

private:
    WiFiClientSecure client;
    HTTPClient http;

public:
    class Request {
    public:
        Request(
			const String (*getMessage)(const char**),
			const char** headers,
			const char* endpoint
		);
        const String execute(
			WiFiClientSecure& client,
			HTTPClient& http,
			const char** data
		);

    private:
        const String (*getMessage)(const char**);
        const char** headers;
		const char*	 endpoint;
    };
};

/*
	To register compile time constant requests, use the following syntax:
	constexpr char requestAuthorize[] = "authorize";

	const char *endpoint = "your_endpoint_here"
	template <>
	struct RequestMap<requestAuthorize> {
		static HTTPInterface::Request value;
	};

	extern const char* authReqHeaders[];

	HTTPInterface::Request RequestMap<requestAuthorize>::value = HTTPInterface::Request(
		[](const char **data) -> const String {
			String body;
			body.reserve(100);
			body += "{\"socket_id\":\"";
			body += data[0];  // First item
			body += "\",\"channel_name\":\"";
			body += data[1];  // Second item
			body += "\"}";
			return body;
		},
		authReqHeaders,
		endpoint
	);
*/

#endif