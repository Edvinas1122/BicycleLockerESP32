#ifndef LOCKER_H
# define LOCKER_H

#include <Arduino.h>
#include <Display.h>
#include <functional>

#define MAX_NAMES_ON_SCREEN 4

class LockerService {

	public:
	LockerService(
		Display *display
	);
	~LockerService();

	void addName(const char *name);
	void removeName(const char *name);

	private:
	std::function<void(const char *)> loggingMethod;
	String names[MAX_NAMES_ON_SCREEN];
	uint8_t index;
	Display *display;
	void displayNames();

};

void handleNetworkTasks(
	bool *run,
	Display *display
);

#endif