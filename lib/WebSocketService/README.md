# WebSocketService

For ESP32

## About

A wrapper for enabling socket connection for ESP32

## Dependencies

Repository:

[https://github.com/gilmaimon/ArduinoWebsockets.git](https://github.com/gilmaimon/ArduinoWebsockets.git)

Other

- **Connection**, ESP32 must be connected to a network.

## Use

### Initiate

Instantiate Object in a global scope.

```cpp
WebSocketService ws(
	socketAddress,
	[](const char* message) {
		Serial.println(message);
	}
);
```

### Register handlers

```cpp
// asssuming const String getResponse(const String &) available
ws.registerEventHandler(
		"client-ping",
		[&wss, mainChannel](const String& message) {
				const String response = getResponse(message);
				ws.send->(response);
			);
	});
```

### Poll

1. Seek reconnect to a socket address server
2. enable hooks - handler function

```c
// assuming esp has established a connection
void loop() {
	ws.poll();
	delay(200);
}
```

## Development

Module shall ensure support of methods:

- registerEventHandler
- send
- poll
- and a constructor

Connection handling, recovery, modularity improvements desired.