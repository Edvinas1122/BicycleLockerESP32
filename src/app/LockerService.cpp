#include "LockerService.h"

LockerService::LockerService(
	void (*lockerSequenceCallBack)(bool status) /* = NULL */
) :
	openRequest(),
	locks({
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
	}),
	lockerSequenceCallBack(lockerSequenceCallBack)
{}

LockerService::~LockerService() {}

// void LockerService::init() {
// 	for (uint8_t i = 0; i < LOCKER_COUNT; i++) {
// 		pinMode(locks[i].pin, OUTPUT);
// 		digitalWrite(locks[i].pin, LOW);
// 	}
// }

bool LockerService::inCommitedOpenSequence() const {
	return !openRequest.isExpired();
}

void LockerService::endOpenSequence() {
	openRequest.timestamp = 0;
}

/*
	intended for network socket hook
*/
bool LockerService::commitOpenSequence(
	const uint8_t lockerNumber,
	const String &requestee
) {
	if (!inCommitedOpenSequence()) {
		openRequest.pin = lockerToPin(lockerNumber);
		openRequest.timestamp = xx_time_get_time();
		openRequest.requestee = requestee;
		return true;
	} else {
		return false;
	}
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
	// Serial.println(xx_time_get_time());
	if (isButtonPressed() && inCommitedOpenSequence()) {
		locks[openRequest.pin].open();
		openRequest.timestamp = 0;
		lockerSequenceCallBack(true);
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

#define START_KICK_TIME 100000
int64_t LockerService::xx_time_get_time() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL) + START_KICK_TIME);
}

LockerService::OpenRequest::OpenRequest() :
	pin(0), timestamp(0) {
		requestee.reserve(32);
}

bool LockerService::OpenRequest::isExpired() const {
	Serial.print("isExpired ");
	Serial.println(timestamp);
	Serial.println(xx_time_get_time() - timestamp);
	return (((xx_time_get_time() - timestamp) > EXPIRATION_TIME_OF_REQUEST));
}

LockerService::Lock::Lock(const uint8_t pin):
	pin(pin), timestamp(0) {}

void LockerService::Lock::open() {
	digitalWrite(pin, HIGH);
	timestamp = xx_time_get_time();
	Serial.print("open ");
	Serial.println(pin);
}

