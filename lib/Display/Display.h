#ifndef DISPLAY_H
# define DISPLAY_H

# include <Arduino.h>
# include <Wire.h>
# include <Adafruit_GFX.h>
# include <Adafruit_SSD1306.h>
# include <list>

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
		void poll();

		void clear();

		void const_message(const char *text);
		void message(const char *text, uint16_t seconds);
		void sequence_message(const char *text, uint16_t seconds);
		void toggle_repeat_on();
		void setMessageSource(String (*source)());

		// void resetCursor();
	private:
		String		constantText;
		uint16_t	cursorX;
		uint16_t	cursorY;
		void displayText(const char *text);
		void displayTextNextLine(const char *text);
		String (*source)();
		class Message {
			public:
				Message(
					const char *text,
					const uint16_t seconds,
					const uint64_t (*xx_time_get_time)(),
					const uint64_t startTime = 0
				);
				virtual ~Message();
				Message(const Message &rhs);

				const bool	expired() const;
				const String &get() const;
				const uint64_t expiry() const;
				void resetExpiry();

			private:
				const String	text;
				const uint16_t	seconds;
				uint64_t		startTime;
				const uint64_t (*xx_time_get_time)();
		};
		std::list<Message> messages;
		std::list<Message>::iterator sequence_message_iterator;
		bool sequenceIsSetToRepeat() const;
		void handle_repeated_sequence();
		void sequence_message_next();
		void sequence_message_reset();
		void toggle_repeat_off();
		static const uint64_t xx_time_get_time();
};

namespace DisplayMods {

	void setRepeatingMessage(Display *display, const char *text, uint16_t seconds);

	void loadingDots(Display *display, const char *text);
	void countdownTimer(Display *display, const char *text, uint8_t seconds);
};

#endif
