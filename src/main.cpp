#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
//#include <ESP8266mDNS.h>
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#include "FastLED.h"
#include <Ticker.h>
#include "block.h"
#include "event/blink.h"
#include "json.h"

#define COLOR_ORDER GRB
#define LED_TYPE WS2812B
#define NUM_LEDS 189
#define DATA_PIN D1

CRGB leds[NUM_LEDS];

const char* ssid = "Klein Tijssink 2.4GHz";
const char* password = "Hoor wie klopt daar kinderen?";

ESP8266WebServer server(80);
Ticker ticker;
Block* block = NULL;

void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!");
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup() {
  block = new Block(leds, NUM_LEDS);

  // LEDs
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  for (int i = 0; i < NUM_LEDS; i++)
    leds[i] = CRGB(255, 0, 0); // default is red

  Serial.begin(115200);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println();
  Serial.print("Connecting");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // if (MDNS.begin("esp8266")) {
  //   Serial.println("MDNS responder started");
  // }

  // Set up server
  server.on("/", handleRoot);
  server.on("/test", HTTPMethod::HTTP_GET, [](){
      server.send(200, "text/plain", "hahaha get");
  });
  server.on("/test", HTTPMethod::HTTP_POST, [](){
      server.send(200, "text/plain", "hahaha post");
  });

  onJSON(server, "/event", [](JsonObject& root){

    JsonError* e;
    EventBlink* ev;
    e = json_EventBlink(root, &ev);

    if (e != NULL) {
      server.send(400, "text/plain", e->tostr());
      delete e;
      return;
    }

    // Initiate the event
    block->event((Event*) ev);
    server.send(200, "text/plain", "event started");
  });

  server.onNotFound(handleNotFound);
  server.begin();

  Serial.println("HTTP server started");
  ticker.attach_ms(20, [](){
    block->render();
    FastLED.show();
  });
}

void loop() {
  server.handleClient();
}
