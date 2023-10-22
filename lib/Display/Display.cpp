#include "Display.h"

Display::Display():
	display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire),
	cursorX(0),
	cursorY(0)
{
	constantText.reserve(40);
}

Display::~Display() {
	display.clearDisplay();
	display.display();
}

void Display::init() {
	if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_PIN)) {
		Serial.println(F("SSD1306 allocation failed"));
		for(;;);
	}
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.setCursor(0, 0);
}

void Display::clear() {
	display.clearDisplay();
	display.display();
}

void Display::displayText(const char *text) {
	display.clearDisplay();
	display.setCursor(0, 0);
	display.println(text);
	display.display();
}

void Display::const_message(const char *text) {
	messages.clear();
	constantText = text;
}

void Display::message(const char *text, uint16_t seconds) {
	messages.clear();
	messages.emplace_back(text, seconds, xx_time_get_time);
}

void Display::sequence_message(
	const char *text,
	uint16_t seconds,
	bool interval
) {
	messages.emplace_back(text, seconds, xx_time_get_time);
}

Display::Message::Message(
	const char *text,
	uint16_t seconds,
	const uint64_t (*xx_time_get_time)(),
	bool interval
):
	text(text),
	seconds(seconds),
	startTime(xx_time_get_time()),
	xx_time_get_time(xx_time_get_time),
	interval(interval),
	started(false)
{}

Display::Message::~Message() {}

Display::Message::Message(const Message &rhs):
	text(rhs.text),
	seconds(rhs.seconds),
	startTime(rhs.startTime),
	xx_time_get_time(rhs.xx_time_get_time),
	interval(rhs.interval),
	started(false)
{}

// Display::Message &Display::Message::operator=(const Message &rhs) {
// 	if(this != &rhs) {
// 		text = rhs.text;
// 		seconds = rhs.seconds;
// 		startTime = rhs.startTime;
// 		xx_time_get_time = rhs.xx_time_get_time;
// 	}
// 	return *this;
// }

const bool Display::Message::expired() {
	if (!started) {
		started = true;
		startTime = this->xx_time_get_time();
	}
	return this->xx_time_get_time() - startTime > seconds * 1000;
}

const bool Display::Message::isInterval() const {
	return interval;
}

const String &Display::Message::get() const {
	return text;
}

void Display::poll() {
	if (!messages.empty() && messages.front().expired()) {
		messages.pop_front();
	}
	if (messages.empty()) {
		displayText(constantText.c_str());
	} else {
		Serial.print("Display::poll() message: ");
		Serial.println(messages.front().get().c_str());
		displayText(messages.front().get().c_str());
	}
}

#define START_KICK_TIME 100000
const uint64_t Display::xx_time_get_time() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL) + START_KICK_TIME);
}
