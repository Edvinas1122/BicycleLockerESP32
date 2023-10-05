#include "LockerService.h"

LockerService::LockerService(): 
	inOperation(false) {}

LockerService::~LockerService() {}

void LockerService::commitOpenSequence(uint8_t lockerNumber) {
	if (inOperation) {
		return;
	}
	inOperation = true;
}

void LockerService::Poll() {}


static int64_t xx_time_get_time() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
}

LockerService::OpenRequest::OpenRequest(const uint8_t locker)
    : pin(lockerToPin(locker)), timestamp(xx_time_get_time()) {}


const uint8_t LockerService::lockerToPin(const uint8_t lockerNumber) {
	switch (lockerNumber) {
		case 1:
			return 4;
		case 2:
			return 5;
		case 3:
			return 6;
		case 4:
			return 7;
		default:
			return 0;
	}
}