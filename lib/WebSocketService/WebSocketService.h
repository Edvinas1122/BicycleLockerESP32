#ifndef WEB_SOCKET_SERVICE_H
# define WEB_SOCKET_SERVICE_H

#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>
#include <functional>
#include <unordered_map>

// template<const char* EventKey>
// struct EventHandlerMap;

class WebSocketService: public websockets::WebsocketsClient
{
	public:
	typedef std::function<void(const String&)> MessageCallback;

	class EventHandler {
		public:
			EventHandler(
				const char *eventKey,
				MessageCallback callback
			): eventKey(eventKey), callback(callback), registered(true) {};
			~EventHandler() {};
			EventHandler(const EventHandler &other) {
				this->eventKey = other.eventKey;
				this->callback = other.callback;
				this->registered = other.registered;
			}
			EventHandler &operator=(const EventHandler &other) {
				this->eventKey = other.eventKey;
				this->callback = other.callback;
				this->registered = other.registered;
				return *this;
			}
			void execute(const String &message);
			bool isEnabled() const { return registered; }
			void setCallback(MessageCallback callback) {
				this->callback = callback;
			};

			const char *eventKey;
		private:
			MessageCallback callback;
			bool registered;
	};
	typedef std::unordered_map<std::string, EventHandler>::iterator EventHandlerIterator;

	public:
	WebSocketService(const String &url);
	virtual ~WebSocketService();

	bool poll();

	void registerEventHandler(const char* eventName, MessageCallback callback) {
		eventHandlerMap.emplace(std::string(eventName), EventHandler(eventName, callback));
	}

	protected:
	void useHandleEvent(const char *eventKey, const String &message) {
		EventHandlerIterator it = eventHandlerMap.find(std::string(eventKey));
		if (it != eventHandlerMap.end()) {
			try {
				it->second.execute(message);
			} catch (const std::exception &e) {
				Serial.println("Exception: " + String(e.what()));
				connected = false;
			}
		} else {
			Serial.println("Unknown event: " + String(eventKey));
		}
	}

	private:
		const String url;
		bool connected;
		void setupEventDriver();
		void handleConnection();
	private:
        std::unordered_map<std::string, EventHandler> eventHandlerMap;
	public:

	class Message {
		public:
		Message(websockets::WebsocketsMessage);
		Message(const char *);

		const String event();
	
		// template <typename Format = String>
		const String message();
		const String getItem(const char *);

		private:
		StaticJsonDocument<256> json;
		// void parseJson();
		// const websockets::WebsocketsMessage msg;
	};
};


#endif