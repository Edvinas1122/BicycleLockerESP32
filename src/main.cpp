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

const char *endpoint = "https://bicycle-share-front-end-git-deviceinterface-edvinas1122.vercel.app/pusher/auth";

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

String postToAuthPusherEndpoint(
	WiFiClientSecure &client,
	HTTPClient &http,
	const char *socket_id,
	const char *channel_name = "ping"
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
	Serial.println(
		String("POST to server: " + String(response_code)).c_str()
	);
	String responseBody = http.getString(); // Get the response body
	http.end();
	return responseBody; // Return the response body
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

bool run = false;
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
	const char *mainChannel = "presence-locker-device";
	client.setInsecure();

	lockerService.subscribeToChannel(
		mainChannel,
		[&client, &http](const String& socket_id) -> String {
		Serial.println("Subscribing to presence-locker-device");
		String response = postToAuthPusherEndpoint(client, http,
							socket_id.c_str(), "presence-locker-device");
		Serial.println(response);
		return response;
	});
	lockerService.registerEventHandler(
		"pusher_internal:subscription_succeeded",
		[](String& message) {
			Serial.print("Subscription succeeded!");
			Serial.println(message);
	});
	lockerService.registerEventHandler(
		"pusher_internal:member_added",
		[](String& message) {
			Serial.print("Member added!");
			Serial.println(message);
	});
	lockerService.registerEventHandler(
		"pusher_internal:member_removed",
		[](String& message) {
			Serial.print("Member removed!");
			Serial.println(message);
	});
	lockerService.registerEventHandler(
		"pusher:error",
		[](String &message) {
			Serial.println("pusher:error Triggered");
			Serial.println(message);
		}
	);
	lockerService.registerEventHandler(
		"client-ping",
		[mainChannel, &lockerService](String& message) {
			Serial.println(message);
			lockerService.sendMessage("client-pong", mainChannel, "{\"message\":\"online\"}");
	});

	int connectionAttempts = 0;
	while (run) {
		if (lockerService.poll()) {
			connectionAttempts = 0;
		} else {
			connectionAttempts++;
			loggingMethod(
				String("Reestablishing connection... in "
					+ String(connectionAttempts) 
					+ " seconds").c_str());
			vTaskDelay(pdMS_TO_TICKS(1000 * connectionAttempts));
		}
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

TaskHandle_t WebSocketTaskHandle = NULL;

void onConnect() {
    // if (WebSocketTaskHandle != NULL) {
    //     vTaskDelete(WebSocketTaskHandle);
    //     WebSocketTaskHandle = NULL;
    // }
	run = true;
	// xTaskCreate(
	// 	handleNetworkTasks,
	// 	"WebSocketTask",
	// 	60000,
	// 	NULL,
	// 	1,
	// 	&WebSocketTaskHandle
	// );
}

void onDisconnect() {
	run = false;
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
	// vTaskDelay(portMAX_DELAY);
	if (run) {
		handleNetworkTasks(NULL);
	}
}