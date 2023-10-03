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

#endif
