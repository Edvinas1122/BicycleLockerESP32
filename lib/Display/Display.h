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
		void sequence_message(const char *text, uint16_t seconds, bool interval = false);

		// void resetCursor();
	private:
		String		constantText;
		uint16_t	cursorX;
		uint16_t	cursorY;
		void displayText(const char *text);
		class Message {
			public:
				Message(
					const char *text,
					uint16_t seconds,
					const uint64_t (*xx_time_get_time)(),
					bool interval = false
				);
				virtual ~Message();
				Message(const Message &rhs);
				// Message &operator=(const Message &rhs);

				const bool	expired();
				const bool	isInterval() const;
				const String &get() const;

			private:
				const String	text;
				const uint16_t	seconds;
				uint64_t		startTime;
				const uint64_t (*xx_time_get_time)();
				const bool		interval;
				bool			started;
		};
		std::list<Message> messages;

		static const uint64_t xx_time_get_time();
};

#endif
