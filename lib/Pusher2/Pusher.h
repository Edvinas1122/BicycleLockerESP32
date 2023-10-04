#ifndef PUSHER_H
# define PUSHER_H

#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <functional>
#include <unordered_map>
#include <ArduinoJson.h>
#include <functional>

template<const char* EventKey>
struct EventHandlerMap;

class WebSocketService: public websockets::WebsocketsClient
{
	public:
	typedef std::function<void(const String&)> MessageCallback;

	public:
	WebSocketService(const char *url);
	virtual ~WebSocketService();

	bool poll();
	virtual void handleEvent(const char *eventKey, const String &message) = 0;
	virtual bool hasEventHandler(const char *eventKey) = 0;

	private:
		const char *url;
		bool connected;
		void setupEventDriver();
		void handleConnection();
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

class PusherService: 
	public websockets::WebsocketsClient 
{
	public:
	typedef std::function<void(const String&)> MessageCallback;

	public:
		PusherService(
			const char *key,
			const char *cluster
		);
		virtual ~PusherService();

		/*
			Must be called in the loop
		*/
		bool poll();

		/*
			Subscribe to a channel
		*/
		void subscribeToChannel(
				const char *,
				std::function<String(const String&)> authCallback = nullptr);
		/*
			Bind to an event
		*/
		void registerEventHandler(
			const char	*eventKey,
			MessageCallback callback
		);
		void sendMessage(
			const char *event,
			const char *channel,
			const char *data
		);

		class EventHandler {
			public:
				EventHandler(
					PusherService *pusher,
					const char *eventKey,
					MessageCallback callback
				);
				~EventHandler();
				void execute(const String &message);
				bool isEnabled() const { return registered; }

			private:
				PusherService *pusher;
				const char *eventKey;
				MessageCallback callback;
				bool registered;

		};

	private:
		String fullUrl;
		std::unordered_map<std::string, MessageCallback> messageHandlers;
		bool	connected;
		String	socket_id;

		void sendSubReq(
				const char *,
				const String &signature = ""
		);
		void setupEventDriver();
		const bool handleConnection();
	protected:
		void handleEvent(const char* eventKey, const String &message);

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