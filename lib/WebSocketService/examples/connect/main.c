#include <WebSocketService.h>
#include <WiFi.h>

/*
	your connection endpoint
*/
const char* socketAddress = "ws://polkadot.webapi.subscan.io/socket";
/*
	Define a WebSocketService object with the following parameters:
		- socketAddress: The address of the websocket server
		- onMessage: A callback function that will be called when a message is received
*/
WebSocketService ws(
	socketAddress,
	[](const char* message) {
		Serial.println(message);
	}
);


/*
	Use your methods to ensure that esp is connected to a network
*/
static void ensureConnected() {
	const char* ssid = "ssid"; //Enter SSID
	const char* password = "password"; //Enter Password

	WiFi.begin(ssid, password);
    for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
    }
    if(WiFi.status() != WL_CONNECTED) {
        Serial.println("No Wifi!");
        return;
    }
	Serial.println("Connected to Wifi, Connecting to server.");
}


/*
	Ensure that esp is connected to a network
*/
void setup() {
	Serial.begin(115200);
	ensureConnected(); // Connect ESP to a network
}

void loop() {
	ws.poll();
	delay(200);
}