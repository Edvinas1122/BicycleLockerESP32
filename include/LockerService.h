#ifndef LOCKER_H
# define LOCKER_H

# include <stdint.h>
# include <sys/time.h>
# include <Arduino.h>

#define MAX_NAMES_ON_SCREEN 4
#define EXPIRATION_TIME_OF_REQUEST 10000
#define EXPIRATION_TIME_OF_LOCK 5000
const int BUTTON_PIN = 34;
# define LOCKER_COUNT 10

class LockerService {

	public:

	typedef std::function<void(bool)> LockerSequenceCallBack;

	public:
	LockerService(
		LockerSequenceCallBack callback = NULL
	);
	~LockerService();

	bool inCommitedOpenSequence() const;
	bool commitOpenSequence(
		const uint8_t lockerNumber,
		const String &requestee = ""
	);
	void endOpenSequence();
	void poll();

	class OpenRequest {
		OpenRequest();
		~OpenRequest() {};

		bool isExpired() const;

		private:
		uint8_t pin;
		int64_t timestamp;
		String requestee;
		friend class LockerService;
	};

	class Lock {
		public:
		Lock(const uint8_t pin);
		~Lock() {};
		void open();
		private:
		const uint8_t pin;
		int64_t timestamp;
		friend class LockerService;
	};

	protected:
	bool isButtonPressed();
	void openLocker();

	private:
	Lock locks[LOCKER_COUNT];
	OpenRequest openRequest;
	LockerSequenceCallBack callback;
	
	void closeExpiredLocks();
	
	static const uint8_t lockerToPin(const uint8_t lockerNumber) {
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
		};
	}
	static int64_t xx_time_get_time();

};

#endif