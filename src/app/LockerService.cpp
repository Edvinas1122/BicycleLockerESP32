#include "LockerService.h"

LockerService::LockerService() :
	openRequest(), locks({
		Lock(4),
		Lock(5),
		Lock(6),
		Lock(7),
		Lock(8),
		Lock(9),
		Lock(10),
		Lock(11),
		Lock(12),
		Lock(13)
	}) {}

LockerService::~LockerService() {}

bool LockerService::inCommitedOpenSequence() const {
	return !openRequest.isExpired();
}

/*
	intended for network socket hook
*/
void LockerService::commitOpenSequence(const uint8_t lockerNumber) {
	openRequest.pin = lockerToPin(lockerNumber);
	openRequest.timestamp = xx_time_get_time();
}

#define SAMPLE_SIZE 32

bool LockerService::isButtonPressed() {
	size_t totalSampleSize = 0;

	for (uint8_t i = 0; i < SAMPLE_SIZE; i++) {
		totalSampleSize += analogRead(BUTTON_PIN);
		delay(1); // 1ms total 32ms
	}
	if (totalSampleSize / SAMPLE_SIZE < 5) {
		return true;
	} else {
		return false;
	}
}

/*
	32ms block
*/
void LockerService::poll() {
	if (isButtonPressed() && inCommitedOpenSequence()) {
		locks[openRequest.pin].open();
		openRequest.timestamp = 0;
	}
	closeExpiredLocks();
}

void LockerService::closeExpiredLocks() {
	for (uint8_t i = 0; i < LOCKER_COUNT; i++) {
		if (locks[i].timestamp != 0 &&
			(xx_time_get_time() - locks[i].timestamp) > EXPIRATION_TIME_OF_LOCK)
		{
			digitalWrite(locks[i].pin, LOW);
			locks[i].timestamp = 0;
		}
	}
}


int64_t LockerService::xx_time_get_time() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
}

LockerService::OpenRequest::OpenRequest() :
	pin(0), timestamp(0) {}

bool LockerService::OpenRequest::isExpired() const {
	return (timestamp && 
		((xx_time_get_time() - timestamp) > EXPIRATION_TIME_OF_REQUEST));
}

LockerService::Lock::Lock(const uint8_t pin):
	pin(pin), timestamp(0) {}

void LockerService::Lock::open() {
	digitalWrite(pin, HIGH);
	timestamp = xx_time_get_time();
	Serial.print("open ");
	Serial.println(pin);
}
