#ifndef LOCKER_H
# define LOCKER_H

#include <Arduino.h>
#include <functional>

#define MAX_NAMES_ON_SCREEN 4

class LockerService {

	public:
	LockerService();
	~LockerService();

	void Open(uint8_t lockerNumber);

	private:
	boolean inOperation;

};

#endif