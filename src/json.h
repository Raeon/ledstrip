#ifndef _JSON_UTIL_H
#define _JSON_UTIL_H

#include <string>
#include <ESP8266WebServer.h>
#include "ArduinoJson.h"
#include "render.h"
#include "event/blink.h"

using namespace std;

typedef void (*TJsonHandlerFunction)(JsonObject& root);

void onJSON(ESP8266WebServer& server, const String& uri, TJsonHandlerFunction handler);

class JsonError {
private:
  String* _msg;
public:
  JsonError(String& msg);
  ~JsonError();
  JsonError* trace(const String& msg);
  const String& tostr();
};

JsonError* json_uint8_t(JsonObject& root, uint8_t* p, const String& key);
JsonError* json_uint16_t(JsonObject& root, uint16_t* p, const String& key);
JsonError* json_RGBA(JsonObject& root, RGBA** p, const String& key);
JsonError* json_EventBlink(JsonObject& root, EventBlink** p);

#endif /* end of include guard: _JSON_UTIL_H */
