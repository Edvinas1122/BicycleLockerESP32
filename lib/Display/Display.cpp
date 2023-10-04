#include "Display.h"

Display::Display():
	display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire),
	cursorX(0),
	cursorY(0)
{}

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

void Display::printNewLine(const char *text) {
	display.println(text);
	// cursorX = 0;
	// cursorY += 1;
	// display.setCursor(cursorX, cursorY);
}

void Display::resetCursor() {
	cursorX = 0;
	cursorY = 0;
	display.setCursor(cursorX, cursorY);
}