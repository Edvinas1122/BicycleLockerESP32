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
    HTTPInterface(
		void (*log)(const char *) = defaultLog
	): 
        client(), log(log) {
            client.setInsecure();
    };

    virtual ~HTTPInterface() {};

	template<const char* RequestName>
	const String post(const char **data) {
		return RequestMap<RequestName>::value.execute(client, data, log);
	};

protected:
	static void defaultLog(const char *message) {
		(void)message;
	};

private:
	WiFiClientSecure client;
	void (*log)(const char *);

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
			const char** data,
			void (*log)(const char *)
		);

	private:
		const String (*getMessage)(const char**);
		const char** headers;
		const char*	 endpoint;
	};
};

#endif