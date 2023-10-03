#ifndef PUSHER_H
# define PUSHER_H

#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <functional>
#include <unordered_map>
#include <ArduinoJson.h>
#include <functional>

class PusherService: 
	public websockets::WebsocketsClient 
{
	public:
	typedef std::function<void(String&)> MessageCallback;

	public:
		PusherService(
			const char *key,
			const char *cluster
		);
		virtual ~PusherService();
		bool poll();
		void subscribeToChannel(
				const char *,
				std::function<String(const String&)> authCallback = nullptr);
		void registerEventHandler(
			const char	*eventKey,
			MessageCallback callback
		);

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
		void registerDefaultChannelHandling();


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