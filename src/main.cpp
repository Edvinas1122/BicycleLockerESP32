#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <Display.h>
#include <Network.h>
// #include <ArduinoWebsockets.h>
#include <WiFiClientSecure.h>
#include <Pusher.h>
#include "env.h"
#include "UserMessage.h"

Display display;

void loggingMethod(const char *message) {
	Serial.println(message);
	display.displayText(message);
}

void postToPusherEndpoint(
	WiFiClientSecure &client,
	HTTPClient &http,
	const char *message,
	const char *event = "ping"
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

void registerHandler(
	PusherService &service,
	WiFiClientSecure &client,
	HTTPClient &http,
	const char *listenEvent,
	const char *respondEvent = "ping",
	String (*handler)(String &msg) = [](String &msg) { return msg; }
) {
	service.registerEventHandler(
		listenEvent,
		[&http, &client, respondEvent, listenEvent, handler](String &msg) {
			Serial.print(listenEvent);
			Serial.println(" --Triggered");
			postToPusherEndpoint(client, http, handler(msg).c_str(), respondEvent);
		}
	);
}

/*
	https://pusher.com/docs/channels/library_auth_reference/pusher-websockets-protocol/#connection-closure
*/
void handleNetworkTasks(
	void *parameter
) {
	PusherService lockerService(
		PUSHER_KEY,
		PUSHER_CLUSTER
	);
    WiFiClientSecure client;
    HTTPClient http;

	client.setInsecure();

	lockerService.subscribeToChannel("locker-device");
	lockerService.registerEventHandler("pusher_internal:subscription_succeeded", [](String& message){
		Serial.println("Subscription succeeded!");
	});
	registerHandler(lockerService, client, http,
		"ping", "pong",
		[](String &msg) {
			display.displayText(UserMessage(msg.c_str()).username().c_str());
			return String("online");
		});
	lockerService.registerEventHandler(
		"testing",
		[&http, &client](String &msg) {
			Serial.println("testing Triggered");
			loggingMethod(msg.c_str());
		}
	);

	int connectionAttempts = 0;
	while (42) {
		if (lockerService.poll()) {
			connectionAttempts = 0;
		} else {
			connectionAttempts++;
			loggingMethod(
				String("Reestablishing connection... in "
					+ String(connectionAttempts) 
					+ " seconds").c_str());
			delay(1000 * connectionAttempts); // should be exponential backoff
		}
		delay(500);
	}
}

TaskHandle_t WebSocketTaskHandle = NULL;

void onConnect() {
	xTaskCreate(
		handleNetworkTasks,
		"WebSocketTask",
		10000,
		NULL,
		1,
		&WebSocketTaskHandle
	);
}

void onDisconnect() {
    if (WebSocketTaskHandle != NULL) {
        vTaskDelete(WebSocketTaskHandle);
        WebSocketTaskHandle = NULL;
    }
}

Network	localNetwork(
			LOCAL_SSID,
			LOCAL_USER,
			LOCAL_PASSWORD,
			loggingMethod,
			onConnect,
			onDisconnect
		);

void setup()
{
    Serial.begin(115200);
	display.init();
	localNetwork.init();
}

void loop()
{

}