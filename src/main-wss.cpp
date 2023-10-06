#include <WebSocketService.cpp>
#include <WiFi.h>


const char* socketAddress = "ws://polkadot.webapi.subscan.io/socket";
const char* ssid = "ssid"; //Enter SSID
const char* password = "password"; //Enter Password
const char* websockets_server_host = "serverip_or_name"; //Enter server adress

WebSocketService wss(
	socketAddress,
	[](const char* message) {
		Serial.println(message);
	}
);

void setup() {
	Serial.begin(115200);
	WiFi.begin(ssid, password);

    for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
    }
	    // Check if connected to wifi
    if(WiFi.status() != WL_CONNECTED) {
        Serial.println("No Wifi!");
        return;
    }
	Serial.println("Connected to Wifi, Connecting to server.");
}

void loop() {
	wss.poll();
	delay(200);
}