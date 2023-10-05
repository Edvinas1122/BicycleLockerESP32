#ifndef LOCKER_H
# define LOCKER_H

#include <Arduino.h>
#include <functional>

#define MAX_NAMES_ON_SCREEN 4

class LockerService {

	public:
	LockerService();
	~LockerService();

	void commitOpenSequence(const uint8_t lockerNumber);
	void init();
	void Poll();

	class OpenRequest {
		OpenRequest(const uint8_t locker);
		~OpenRequest() {};

		private:
		const uint8_t pin;
		int64_t timestamp;
		
	};
	private:
	boolean inOperation;
	static const uint8_t lockerToPin(const uint8_t lockerNumber);

};

#endif