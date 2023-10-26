#include <EventHandling.h>

static const String composeJSON(
	const String &locker_id,
	const int64_t timestamp
) {
	StaticJsonDocument<256> doc;
	doc["locker_id"] = locker_id;
	doc["timestamp"] = timestamp;
	String json;
	serializeJson(doc, json);
	return json;
}

static const String composeSignedJSON(
	const String &data,
	const String &signature
) {
	StaticJsonDocument<1024> doc;
	doc["data"] = data;
	doc["signature"] = signature;
	String json;
	serializeJson(doc, json);
	return json;
}

LockerService::OpenRequest
	::SignMethod getSignatureMethod(
		Cypher *cypher
	) {
		return [cypher](const String &locker_id)->String {
			Serial.println("getSignatureMethod");
			const String json = composeJSON(
				locker_id,
				xx_time_get_time()
			);
			Serial.println(json);
			const String signature = cypher->sign(json.c_str());
			Serial.println(signature);
			const String signed_json = composeSignedJSON(
				json,
				signature
			);
			Serial.println(signed_json);
			return signed_json;
		};
	}