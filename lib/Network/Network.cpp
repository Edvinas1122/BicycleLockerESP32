#include "Network.h"

Network::Network(
	const char *ssid,
	const char *username,
	const char *password,
	void (*log)(const char *message),
	void (*onConnect)(void),
	void (*onDisconnect)(void)
) : ssid(ssid), username(username), password(password),
	log(log), onConnect(onConnect), onDisconnect(onDisconnect) {}

void Network::init() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    
	WiFi.onEvent([this](arduino_event_id_t event, arduino_event_info_t info) {
		if(event == ARDUINO_EVENT_WIFI_STA_GOT_IP) {
			String eventMessage = String("Connected to AP. Got IP: ") + WiFi.localIP().toString();
			this->log(eventMessage.c_str());
			this->onConnect();
		} else if(event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED) {
			this->log("Disconnected from AP.");
			WiFi.reconnect();
		}
	});
    
	/*
		For enterprise WPA2 networks
	*/
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)username, strlen(username));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)username, strlen(username));
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)password, strlen(password));
    esp_wifi_sta_wpa2_ent_enable();
    
    WiFi.begin(ssid, password, 0, NULL, username);
}

boolean Network::isConnected() {
	return WiFi.isConnected();
}

void Network::defaultLog(const char *message) {
	Serial.println(message);
}

// const char* Network::getWiFiEventDescription(WiFiEvent_t event) {
//     switch (event) {
//         case SYSTEM_EVENT_WIFI_READY: return "WiFi interface ready";
//         case SYSTEM_EVENT_SCAN_DONE: return "Completed scan for access points";
//         case SYSTEM_EVENT_STA_START: return "WiFi client started";
//         case SYSTEM_EVENT_STA_STOP: return "WiFi clients stopped";
//         case SYSTEM_EVENT_STA_CONNECTED: return "Connected to access point";
//         case SYSTEM_EVENT_STA_DISCONNECTED: return "Disconnected from WiFi access point";
//         case SYSTEM_EVENT_STA_AUTHMODE_CHANGE: return "Authentication mode of access point has changed";
//         case SYSTEM_EVENT_STA_GOT_IP: return "Obtained IP address";
//         case SYSTEM_EVENT_STA_LOST_IP: return "Lost IP address and the IP address is reset to 0";
//         case SYSTEM_EVENT_STA_WPS_ER_SUCCESS: return "WiFi Protected Setup (WPS): succeeded in enrollee mode";
//         case SYSTEM_EVENT_STA_WPS_ER_FAILED: return "WiFi Protected Setup (WPS): failed in enrollee mode";
//         case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT: return "WiFi Protected Setup (WPS): timeout in enrollee mode";
//         case SYSTEM_EVENT_STA_WPS_ER_PIN: return "WiFi Protected Setup (WPS): pin code in enrollee mode";
//         case SYSTEM_EVENT_AP_START: return "WiFi access point started";
//         case SYSTEM_EVENT_AP_STOP: return "WiFi access point  stopped";
//         case SYSTEM_EVENT_AP_STACONNECTED: return "Client connected to WiFi access point";
//         case SYSTEM_EVENT_AP_STADISCONNECTED: return "Client disconnected from WiFi access point";
//         case SYSTEM_EVENT_AP_STAIPASSIGNED: return "Assigned IP address to client";
//         case SYSTEM_EVENT_AP_PROBEREQRECVED: return "Received probe request";
//         case SYSTEM_EVENT_GOT_IP6: return "IPv6 is preferred";
//         case SYSTEM_EVENT_ETH_START: return "Ethernet started";
//         case SYSTEM_EVENT_ETH_STOP: return "Ethernet stopped";
//         case SYSTEM_EVENT_ETH_CONNECTED: return "Ethernet connected";
//         case SYSTEM_EVENT_ETH_DISCONNECTED: return "Ethernet disconnected";
//         case SYSTEM_EVENT_ETH_GOT_IP: return "Obtained IP address";
//         default: return "Unknown event";
//     }
// }