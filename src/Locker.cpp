#include "Locker.h"

LockerService::LockerService(): 
	inOperation(false) {}

LockerService::~LockerService() {}

void LockerService::Open(uint8_t lockerNumber) {
	if (inOperation) {
		return;
	}
	inOperation = true;
}
