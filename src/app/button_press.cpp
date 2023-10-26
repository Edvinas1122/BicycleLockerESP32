#include <EventHandling.h>

const String SequenceReport(const bool unlocked) {
	return String("{\"message\":") + (unlocked ? "\"unlocked\"" : "\"timedout\"") + "}";
}

const String SequenceReportDisplay(const bool unlocked) {
	return String("Locker ") + (unlocked ? "unlocked" : "timedout");
}

const String userEvent(const char *event, const String &id) {
	String result = String(event) + "-" + id;
	return result;
}

/*
	Callback for button press event
*/
LockerService::ButtonPressCallBack getButtonPressCallback(
	PusherService& socket,
	Display* display,
	const char* mainChannel
) {
	return [&socket, mainChannel, display](
		const bool unlocked,
		const String &signature_json,
		const String &requestee_id
	) {
		if (unlocked) {
			socket.sendMessage(
				userEvent("client-button-signature", requestee_id).c_str(),
				mainChannel,
				signature_json.c_str()
			);
			DisplayMods::loadingDots(display, "Button pressed, standby");
		} else {
			display->message(
				"Locker timed out",
				3
			);
		}
		Serial.println("Button pressed");
	};
}