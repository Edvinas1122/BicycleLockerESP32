#include "Locker.h"

LockerService::LockerService(
	Display *display
) {
	this->display = display;
	for (int i = 0; i < MAX_NAMES_ON_SCREEN; i++) {
		names[i].reserve(16);
	}
	index = 0;
}

LockerService::~LockerService() {}

void LockerService::addName(const char *name) {
	names[index++] = name;
	if (index >= MAX_NAMES_ON_SCREEN) {
		index = 0;
	}
	displayNames();
}

void LockerService::removeName(const char *name) {
	for (int i = 0; i < MAX_NAMES_ON_SCREEN; i++) {
		if (names[i] == name) {
			names[i] = "";
		}
	}
	displayNames();
}

void LockerService::displayNames()
{
	display->resetCursor();
	display->clear();
	for (int i = 0; i < MAX_NAMES_ON_SCREEN; i++) {
		if (names[i] != "") {
			display->displayText(names[i].c_str());
		}
	}
}