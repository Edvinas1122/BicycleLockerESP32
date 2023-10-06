// #include "../Pusher.h"

// const char *PusherService::event(PusherService::Events event) {
// 	switch (event) {
// 		case PusherService::Events::CONNECTION_ESTABLISHED:
// 			return "pusher:connection_established";
// 		case PusherService::Events::SUBSCRIPTION_SUCCESS:
// 			return "pusher_internal:subscription_succeeded";
// 		case PusherService::Events::SUBSCRIPTION_ERROR:
// 			return "pusher_internal:subscription_error";
// 		case PusherService::Events::MEMBER_ADDED:
// 			return "pusher_internal:member_added";
// 		case PusherService::Events::MEMBER_REMOVED:
// 			return "pusher_internal:member_removed";
// 		case PusherService::Events::CLIENT_EVENT:
// 			return "client-";
// 		case PusherService::Events::ERROR:
// 			return "pusher:error";
// 		case PusherService::Events::UNKNOWN:
// 			return "unknown";
// 		default:
// 			return "unknown";

// 	}
// }