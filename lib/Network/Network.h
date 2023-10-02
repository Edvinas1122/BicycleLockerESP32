#ifndef NETWORK_H
# define NETWORK_H

# include <Arduino.h>
# include <WiFi.h>
# include <HTTPClient.h>
# include <ArduinoJson.h>
# include "esp_wpa2.h"

/*ssid in constructor*/

/*
	For enterprise WPA2 networks
*/
class Network {
	private:
		const char *ssid;
		const char *username;
		const char *password;
		void (*log)(const char *message);
		void (*onConnect)(void);
		void (*onDisconnect)(void);
	public:
	Network(
		const char *ssid,
		const char *username,
		const char *password,
		void (*log)(const char *message) = defaultLog,
		void (*onConnect)(void) = defaultOnConnect,
		void (*onDisconnect)(void) = defaultOnConnect
	);
	void init();
	boolean isConnected();

	private:
		static void defaultLog(const char *message);
		static void defaultOnConnect(void) {};
		static void defaultOnDisconnect(void) {};
		static const char* getWiFiEventDescription(WiFiEvent_t event);
};

// #include <WebSocketsClient.h>
// #include "ESP8266-Websocket/WebSocketClient.h"

// class WebSocket {
// 	public:
// 		typedef struct Url_s {
// 			const char *host;
// 			const uint16_t port;
// 			const char *url;
// 			Url_s(const char *h, uint16_t p, const char *u) : host(h), port(p), url(u) {}
// 		} Url;

// 	private:
// 		// WebSocketsClient webSocket;
// 		WebSocketClient webSocket;
// 		Url url;
// 		// void (*onEvent)(WStype_t type, uint8_t *payload, size_t length);


// 	public:
// 		WebSocket(
// 			Url urlParam
// 			// void (*onEvent)(WStype_t type, uint8_t *payload, size_t length)
// 		);
// 		void init();
// 		boolean isConnected();
// 		// void send(const char *message);
// 		// void send(JsonObject &json);
// 		// void send(JsonArray &json);
// };

#endif
