#include "Display.h"

void setRepeatingMessage(
	Display *display,
	const char *messages[],
	int seconds
) {
	uint8_t i = 0;

	display->message(messages[i], seconds);
	while (messages[++i] != NULL) {
		display->sequence_message(messages[i], seconds);
	}
	display->toggle_repeat_on();
}

#define LOADING_DOTS_COUNT 4
#define EXPECTED_MAX_MESSAGE_LENGTH 20

void DisplayMods::loadingDots(Display *display, const char *text) {
	char temp_array[LOADING_DOTS_COUNT][EXPECTED_MAX_MESSAGE_LENGTH];
	char *array[LOADING_DOTS_COUNT + 1];
	uint8_t i = 0;

	while (i < LOADING_DOTS_COUNT)
	{
		memcpy(temp_array[i], text, strlen(text));
		memset(temp_array[i] + strlen(text), '.', i);
		temp_array[i][strlen(text) + i + 1] = '\0';
		i++;
	}
	i = 0;
	while (i < LOADING_DOTS_COUNT)
	{
		array[i] = temp_array[i];
		i++;
	}
	array[i] = NULL;
	setRepeatingMessage(display, (const char **)array, 1);
}

void DisplayMods::countdownTimer(
	Display *lockerDisplay,
	const char *prefix,
	const uint8_t seconds
) {
	int timeLeft = seconds;
	String message = prefix;
	lockerDisplay->message((message + String(timeLeft)).c_str(), 1);
	while (timeLeft > 0) {
		timeLeft--;
		lockerDisplay->sequence_message(
			(message + String(timeLeft)).c_str(),
			1
		);
	}
	lockerDisplay->sequence_message("Timed out...", 3);
}