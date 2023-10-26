#include "Display.h"

Display::Display():
	display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire),
	cursorX(0),
	cursorY(0),
	sequence_message_iterator(messages.end()),
	source(nullptr)
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
	display.setCursor(0, 10);
	display.println(text);
	// display.display();
}

void Display::setMessageSource(String (*source)()) {
	this->source = source;
}

void Display::displayTextNextLine(const char *text) {
	display.setCursor(0, 20);
	display.println(text);
	// display.display();
}

void Display::const_message(const char *text) {
	messages.clear();
	toggle_repeat_off();
	constantText = text;
}

void Display::message(
	const char *text,
	uint16_t seconds
) {

	messages.clear();
	toggle_repeat_off();
	messages.emplace_back(
		text,
		seconds,
		xx_time_get_time,
		xx_time_get_time()
	);
}

void Display::sequence_message(
	const char *text,
	uint16_t seconds
) {
	const uint64_t previous_expires_at = messages.back().expiry();
	messages.emplace_back(
		text,
		seconds,
		xx_time_get_time,
		previous_expires_at
	);
}

bool Display::sequenceIsSetToRepeat() const {
	if (messages.empty()) {
		return false;
	}
	return sequence_message_iterator != messages.end();
}

Display::Message::Message(
	const char *text,
	const uint16_t seconds,
	const uint64_t (*xx_time_get_time)(),
	const uint64_t startTime
):
	text(text),
	seconds(seconds),
	startTime(startTime),
	xx_time_get_time(xx_time_get_time)
{
	if (startTime == 0) {
		this->startTime = xx_time_get_time();
	}
}

Display::Message::~Message() {}

Display::Message::Message(const Message &rhs):
	text(rhs.text),
	seconds(rhs.seconds),
	startTime(rhs.startTime),
	xx_time_get_time(rhs.xx_time_get_time)
{}

const bool Display::Message::expired() const {
	return this->xx_time_get_time() - startTime > seconds * 1000;
}

const String &Display::Message::get() const {
	return text;
}

const uint64_t Display::Message::expiry() const {
	return startTime + seconds * 1000;
}

/*
	for repeated sequences.
*/
void Display::Message::resetExpiry() {
	startTime = xx_time_get_time();
}

void Display::poll() {
	if (sequenceIsSetToRepeat()) {
		handle_repeated_sequence();
		return;
	}
	if (!messages.empty() && messages.front().expired()) {
		messages.pop_front();
	}
	if (messages.empty()) {
		displayText(constantText.c_str());
		if (source != NULL) {
			displayTextNextLine(source().c_str());
		}
	} else {
		displayText(messages.front().get().c_str());
	}
	display.display();
}

void Display::sequence_message_reset() {
	sequence_message_iterator = messages.begin();
}

void Display::sequence_message_next() {
	sequence_message_iterator++;
	if (sequence_message_iterator == messages.end()) {
		sequence_message_iterator = messages.begin();
	}
}

void Display::handle_repeated_sequence() {
	if (sequence_message_iterator->expired()) {
		sequence_message_next();
		sequence_message_iterator->resetExpiry();
		displayText(sequence_message_iterator->get().c_str());
	}
}

void Display::toggle_repeat_on() {
	sequence_message_reset();
}

void Display::toggle_repeat_off() {
	sequence_message_iterator = messages.end();
}

#define START_KICK_TIME 100000
const uint64_t Display::xx_time_get_time() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL) + START_KICK_TIME);
}
