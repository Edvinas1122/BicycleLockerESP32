#include <utils.h>

const int64_t xx_time_get_time() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
}

void printLocalTime()
{
	struct tm timeinfo;
	if(!getLocalTime(&timeinfo)){
		Serial.println("Failed to obtain time");
		return;
	}
	Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

String getLocalTime()
{
	struct tm timeinfo;
	char buffer[64]; // Adjust the buffer size as needed

	if(!getLocalTime(&timeinfo)){
		Serial.println("Failed to obtain time");
		return String("Failed to obtain time");
	}

	strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);
	return String(buffer);
}

const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

const char* ntpServer = "pool.ntp.org";

void fetchLocalTime() {
	configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
	printLocalTime();
}