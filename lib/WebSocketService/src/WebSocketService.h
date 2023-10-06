#ifndef WEB_SOCKET_SERVICE_H
# define WEB_SOCKET_SERVICE_H

#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>
#include <functional>
#include <unordered_map>


class WebSocketService: public websockets::WebsocketsClient
{
	public:
	typedef std::function<void(const String&)> MessageCallback;

	class EventHandler {
		public:
		EventHandler(
			const char *eventKey,
			MessageCallback callback
		);
		~EventHandler() {};
		EventHandler(const EventHandler &other);
		EventHandler &operator=(const EventHandler &other);

		void execute(const String &message);
		bool isEnabled() const;
		void setCallback(MessageCallback callback);

		private:
		const char *eventKey;
		MessageCallback callback;
		bool registered;
	};
	typedef std::unordered_map<std::string, EventHandler>::iterator EventHandlerIterator;

	public:
	WebSocketService(
		const String &url,
		void (*log)(const char *) = defaultLog
	);
	virtual ~WebSocketService();

	bool poll();

	void registerEventHandler(const char* eventName, MessageCallback callback) {
		eventHandlerMap.emplace(std::string(eventName), EventHandler(eventName, callback));
	}

	protected:
	void useHandleEvent(const char *eventKey, const String &message);	
	static void defaultLog(const char *message);

	public:

	struct MessageTraits {
		virtual const String event() = 0;
		virtual const String message() = 0;
		// virtual const String reducer() = 0;
	};

	class Message: public MessageTraits {
		public:
		Message(websockets::WebsocketsMessage);
		Message(const char *);

		const String event() override;
		const String message() override;
		const String getItem(const char *);
		// const String reducer() override;

		protected:
		StaticJsonDocument<256> json;
	};

	private:
		const String url;
		bool connected;
		template<class Message = WebSocketService::Message>
		void setupEventDriver();
		void handleConnection();
		void (*log)(const char *);

	private:
        std::unordered_map<std::string, EventHandler> eventHandlerMap;
};

#endif