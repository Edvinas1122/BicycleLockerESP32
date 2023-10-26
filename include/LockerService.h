#ifndef LOCKER_H
# define LOCKER_H

# include <stdint.h>
# include <utils.h>
# include <Arduino.h>

#define MAX_NAMES_ON_SCREEN 4
#define EXPIRATION_TIME_OF_REQUEST 10000
#define EXPIRATION_TIME_OF_LOCK 5000
const int BUTTON_PIN = 34;
# define LOCKER_COUNT 10

class LockerService {

	public:
	class OpenRequest {
		public:
		typedef std::function<String(
			const String &locker_id
		)> SignMethod;

		OpenRequest(
			const SignMethod signMethod
		);
		~OpenRequest() {};

		/*
			aka. is available
		*/
		bool isExpired() const;	

		void set(
			const String &locker_id,
			const String &requestee
		);
		String respond();
		bool cancel(const String &requestee);
		const String &getRequestee() const;
		private:
		int64_t			timestamp;
		String			locker_id;
		String			requestee;
		SignMethod		signMethod;
		void expire();
	};
	typedef std::function<void(
		bool unlocked,
		const String& signature_json,
		const String& requestee
	)> ButtonPressCallBack;

	public:
	LockerService(
		ButtonPressCallBack callback = NULL,
		OpenRequest::SignMethod signMethod = NULL
	);
	~LockerService();

	void poll();
	void init();

	bool available() const;
	bool registerRequest(
		const String &locker_id,
		const String &requestee
	);
	bool cancelRequest(
		const String &requestee
	);

	void unlock(const String &lock_id);
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
	OpenRequest openRequest;
	Lock locks[LOCKER_COUNT];
	ButtonPressCallBack buttonPressCallback;
	
	void closeExpiredLocks();


};

#endif