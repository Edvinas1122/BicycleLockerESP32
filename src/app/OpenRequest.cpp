#include "LockerService.h"

LockerService::OpenRequest::OpenRequest(
	const SignMethod signMethod
) :
	timestamp(0),
	requestee(),
	signMethod(signMethod)
{}

bool LockerService::OpenRequest::isExpired() const {
	return xx_time_get_time() - timestamp > EXPIRATION_TIME_OF_REQUEST;
}

void LockerService::OpenRequest::set(
	const String &locker_id,
	const String &requestee
) {
	this->requestee = requestee;
	this->locker_id = locker_id;
	this->timestamp = xx_time_get_time();
}

String LockerService
	::OpenRequest::respond() {
		this->expire();
		Serial.println("respond");
		Serial.println(locker_id);
		const String response = signMethod(locker_id);
		return response;
	}

bool LockerService
	::OpenRequest::cancel(const String &requestee) {
		Serial.println("cancel");
		if (this->requestee == requestee) {
			Serial.println("cancel");
			this->expire();
			return true;
		} else {
			Serial.println("cancel not match");
			return false;
		}
	}

const String &LockerService::OpenRequest::getRequestee() const {
	return requestee;
}


void LockerService::OpenRequest::expire() {
	this->timestamp = 0;
}