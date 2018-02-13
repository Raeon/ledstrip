#ifndef _JSON_UTIL_H
#define _JSON_UTIL_H

#include "ArduinoJson.h"
#include <ESP8266WebServer.h>
#include <string>

using namespace std;

typedef void (*JsonHandler)(JsonObject& root);

void onJSON(ESP8266WebServer& server, const String& uri, JsonHandler handler);
void onJSON(ESP8266WebServer& server, const String& uri, HTTPMethod method, JsonHandler handler);

class JsonError {
private:
    String* _msg;

public:
    JsonError(String& msg);
    ~JsonError();
    JsonError* trace(const String& msg);
    const String& tostr();
};

JsonError* json_object(JsonObject& root, const String& key);
JsonError* json_size_t(JsonObject& root, size_t* p, const String& key);
JsonError* json_uint8_t(JsonObject& root, uint8_t* p, const String& key);
JsonError* json_uint16_t(JsonObject& root, uint16_t* p, const String& key);

#endif /* end of include guard: _JSON_UTIL_H */
