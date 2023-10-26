#include "LockerService.h"

LockerService::LockerService(
	ButtonPressCallBack callback, /* = NULL */
	OpenRequest::SignMethod signMethod /* = NULL */
) :
	openRequest(signMethod),
	locks({
		Lock(2),
		Lock(4),
		Lock(5),
		Lock(13),
		Lock(14),
		Lock(15),
		Lock(16),
		Lock(17),
		Lock(18),
		Lock(19)
	}),
	buttonPressCallback(callback)
{}

LockerService::~LockerService() {}

void LockerService::init() {
	for (uint8_t i = 0; i < LOCKER_COUNT; i++) {
		pinMode(locks[i].pin, OUTPUT);
		digitalWrite(locks[i].pin, LOW);
		locks[i].timestamp = 0;
	}
}

/*
	intended for network socket hook
*/
bool LockerService::registerRequest(
	const String &locker_id,
	const String &requestee
) {
	if (openRequest.isExpired()) {
		openRequest.set(
			locker_id,
			requestee
		);
		return true;
	} else {
		return false;
	}
}

bool LockerService::cancelRequest(
	const String &requestee
) {
	if (!openRequest.isExpired()) {
		return openRequest.cancel(requestee);
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

bool LockerService::available() const {
	return openRequest.isExpired();
}

/*
	32ms block
*/
void LockerService::poll() {
	// Serial.println(xx_time_get_time());
	if (isButtonPressed() && !openRequest.isExpired()) {
		const String response = openRequest.respond();
		buttonPressCallback(
			true,
			response,
			openRequest.getRequestee()
		);
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

void LockerService::unlock(const String &locker_id) {
	uint8_t lockerIndex = locker_id.toInt();
	locks[lockerIndex].open();
}

LockerService::Lock::Lock(const uint8_t pin):
	pin(pin), timestamp(0) {}

void LockerService::Lock::open() {
	// digitalWrite(pin, HIGH);
	timestamp = xx_time_get_time();
	Serial.print("open ");
	Serial.println(pin);
}

