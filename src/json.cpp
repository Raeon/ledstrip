#include "json.h"

using namespace std;

JsonError::JsonError(String& msg) {
  this->_msg = new String(msg);
}

JsonError::~JsonError() {
  delete this->_msg;
}

JsonError* JsonError::trace(const String& msg) {
  String* m = new String(msg);
  m->concat("\n");
  m->concat(*this->_msg);
  delete this->_msg;
  this->_msg = m;
  return this;
}

const String& JsonError::tostr() {
  return *(this->_msg);
}

void onJSON(ESP8266WebServer& server, const String& uri, JsonHandler handler) {
  onJSON(server, uri, HTTPMethod::HTTP_GET, handler);
}

void onJSON(ESP8266WebServer& server, const String& uri, HTTPMethod method, JsonHandler handler) {
 server.on(uri, method, [&server, handler](){
   // Check if body is specified
   if (!server.hasArg("plain")) {
     server.send(400, "text/plain", "no JSON body given");
     return;
   }

   // Get URI and body
   String uri = server.uri();
   String body = server.arg("plain");

   // Parse the JSON
   StaticJsonBuffer<1024> buff;
   JsonObject& root = buff.parseObject(body);

   if (!root.success()) {
     server.send(400, "text/plain", "illegal JSON");
     return;
   }

   // Invoke the real handler
   (*handler)(root);
 });
}

JsonError* json_uint8_t(JsonObject& root, uint8_t* p, const String& key) {
  if (!root.containsKey(key)) return new JsonError("uint8_t missing: " + key);
  *p = root[key].as<uint8_t>();
  return NULL;
}

JsonError* json_uint16_t(JsonObject& root, uint16_t* p, const String& key) {
  if (!root.containsKey(key)) return new JsonError("uint16_t missing: " + key);
  *p = root[key].as<uint16_t>();
  return NULL;
}

JsonError* json_RGBA(JsonObject& root, RGBA** p, const String& key) {
  if (!root.containsKey(key))
    return new JsonError("key missing: " + key);

  uint8_t r, g, b, a;
  JsonError* e;
  JsonObject& obj = root[key];

  if ((e = json_uint8_t(obj, &r, "r"))
    || (e = json_uint8_t(obj, &g, "g"))
    || (e = json_uint8_t(obj, &b, "b"))
    || (e = json_uint8_t(obj, &a, "a")))
    return e->trace("when parsing RGBA: " + key);

  *p = new RGBA(r, g, b, a);
  return NULL;
}

JsonError* json_EventBlink(JsonObject& root, EventBlink** p) {
  JsonError* e = NULL;

  RGBA* color;
  uint8_t times;
  uint8_t length;

  if ((e = json_RGBA(root, &color, "color"))
    || (e = json_uint8_t(root, &times, "times"))
    || (e = json_uint8_t(root, &length, "length")))
    return e->trace("when parsing EventBlink: ");

  *p = new EventBlink(color, times, length);
  return NULL;
}
