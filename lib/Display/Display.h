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
		void init();
		void clear();
		void displayText(const char *text);
};

typedef struct SequenceUnit_s {
	const char *text;
	int duration;
} SequenceUnit;

class TextSequence {
	private:
		SequenceUnit *sequence;
		int sequenceLength;
		int currentUnit;
	public:
		TextSequence(SequenceUnit *sequence, int sequenceLength);
		SequenceUnit getSequenceUnit();
};

#include <freertos/FreeRTOS.h>

class DisplayTask {
	private:
		Display *display;
		TextSequence *sequence;
		TaskHandle_t taskHandle;

	public:
		DisplayTask(Display *display, TextSequence *sequence);
		void start();
		void stop();
		static void task(void *parameter);
};

#endif