// #include "Network.h"

// WebSocket::WebSocket(
// 	Url urlParam,
// 	void (*onEvent)(WStype_t type, uint8_t *payload, size_t length)
// ):	url(urlParam),
// 	onEvent(onEvent) {}

// void WebSocket::init() {
// 	Serial.println("WebSocket::init");
// 	webSocket.onEvent(onEvent);
// 	webSocket.begin(url.host, url.port, url.url, "");
// 	webSocket.hanshake();
// }

// boolean WebSocket::isConnected() {
// 	return webSocket.isConnected();
// }