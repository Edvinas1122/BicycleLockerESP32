#ifndef DISPLAY_H
# define DISPLAY_H

# include <Arduino.h>
# include <Wire.h>
# include <Adafruit_GFX.h>
# include <Adafruit_SSD1306.h>

# define SCREEN_WIDTH 128
# define SCREEN_HEIGHT 64
# define OLED_PIN 0x3C

class Display {
	private:
		Adafruit_SSD1306 display;

	public:
		Display();
		virtual ~Display();
		void init();
		void clear();
		void displayText(const char *text);
		void printNewLine(const char *text);
		void resetCursor();
	private:
		uint16_t cursorX;
		uint16_t cursorY;
};

#endif