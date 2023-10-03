#ifndef PUSHER_H
# define PUSHER_H

#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <functional>
#include <unordered_map>
#include <ArduinoJson.h>

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
		virtual ~PusherService() {};
		bool poll();
		void subscribeToChannel(const char *);
		void registerEventHandler(
			const char	*eventKey,
			MessageCallback callback
		);

	private:
		String fullUrl;
		std::unordered_map<std::string, MessageCallback> messageHandlers;
		bool connected;

		void sendSubReq(const char *);
		void setupEventDriver();
	
		const bool handleConnection();
		void registerDefaultChannelHandling();

	public:

	class Message {
		public:
		Message(websockets::WebsocketsMessage);

		const String event();
	
		// template <typename Format = String>
		const String message();

		private:
		StaticJsonDocument<256> json;
		// void parseJson();
		// const websockets::WebsocketsMessage msg;
	};
};

#endif