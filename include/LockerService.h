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

	typedef std::function<void(
		bool, const String&, const String&, const String&, const String&
	)> LockerSequenceCallBack;

	public:
	LockerService(
		LockerSequenceCallBack callback = NULL
	);
	~LockerService();

	bool inCommitedOpenSequence() const;
	bool commitOpenSequence(
		const uint8_t lockerNumber,
		const String &purpose,
		const String &requestee,
		const String &duration = ""
	);
	void endOpenSequence();
	void poll();
	void init();
	bool requestCancelPermit(
		const String &requestee
	);

	class OpenRequest {
		OpenRequest();
		~OpenRequest() {};

		bool isExpired() const;
		void set(
			const uint8_t pin,
			const int64_t timestamp,
			const String &requestee,
			const String &duration,
			const String &purpose
		);

		private:
		uint8_t pin;
		int64_t timestamp;
		String requestee;
		String duration;
		String purpose;
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
	LockerSequenceCallBack buttonPressCallback;
	
	void closeExpiredLocks();
	
	static int64_t xx_time_get_time();

};

#endif