#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <Display.h>
#include <Network.h>
// #include <ArduinoWebsockets.h>
#include <Pusher.h>
#include "env.h"

Display display;

void loggingMethod(const char *message) {
	Serial.println(message);
	display.displayText(message);
}

void postToServer(
	WiFiClient &client,
	HTTPClient &http,
	const char *message
) {
	http.begin(client, "https://bicycle-share-front-end.vercel.app/pusher/update");
	http.addHeader("Content-Type", "application/json");
	http.addHeader("x-password", PUSHER_KEY);
	String body = "{\"event\":\"observe\",\"message\":\"";
		body += message;
		body += "\"}";
	const int response_code = http.POST(body);
	if (response_code != 200) {
		loggingMethod(
			String("Failed to POST to server: " + String(response_code)).c_str()
		);
	}
	http.end();
}
/*
	https://pusher.com/docs/channels/library_auth_reference/pusher-websockets-protocol/#connection-closure
*/
void handleNetworkTasks(void *parameter) {
	PusherService lockerService(
		PUSHER_KEY,
		PUSHER_CLUSTER
	);
    WiFiClient client;
    HTTPClient http;
	// client.setInsecure();
	postToServer(client, http, "online");
	lockerService.subscribeToChannel("locker-device");
	lockerService.registerEventHandler(
		"observe",
		[&http, &client](websockets::WebsocketsMessage msg) {
			loggingMethod(msg.data().c_str());
			postToServer(client, http, "online");
		}
	);

	int connectionAttempts = 0;
	while (42) {
		if (lockerService.poll()) {
			connectionAttempts = 0;
		} else {
			connectionAttempts++;
			loggingMethod(
				String("Reestablishing connection... in " + String(connectionAttempts) + " seconds").c_str()
			);
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