#ifndef PUSHER_H
# define PUSHER_H

#include <ArduinoWebsockets.h>
#include <functional>
#include <unordered_map>

class PusherService: 
	public websockets::WebsocketsClient 
{

	public:
	typedef std::function<void(websockets::WebsocketsMessage)> MessageCallback;

	public:
		PusherService(
			const char *key,
			const char *cluster
		);
		virtual ~PusherService() {};
		bool poll();
		void subscribeToChannel(const char *channel);
		void registerEventHandler(
			const char	*eventKey,
			MessageCallback callback
		);

	private:
		String fullUrl;
		const char *channel;
		std::unordered_map<std::string, MessageCallback> messageHandlers;

		void handleConnection();
		void handleSubscription();
};

#endif