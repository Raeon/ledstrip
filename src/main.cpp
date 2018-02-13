#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//#include <ESP8266mDNS.h>
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#include "FastLED.h"
#include "block.h"
#include "event/blink.h"
#include "json.h"
#include <Ticker.h>

#include <unordered_map>

using namespace std;

#define COLOR_ORDER GRB
#define LED_TYPE WS2812B
#define NUM_LEDS 189
#define DATA_PIN D1

CRGB leds[NUM_LEDS];

const char* ssid     = "Klein Tijssink 2.4GHz";
const char* password = "Hoor wie klopt daar kinderen?";

ESP8266WebServer server(80);
Ticker ticker;

unordered_map<size_t, Block*> blocks;

void setup() {

    // Do not add any LEDs or blocks by default;
    // they have to be added through the API.
    // Block* block = new Block(leds, NUM_LEDS);
    // blocks.insert(block, block);

    // LEDs
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(50);
    for (int i = 0; i < NUM_LEDS; i++)
       leds[i] = CRGB(255, 0, 0); // default is RED
    
    // Default block
    Block* block = new Block(leds, NUM_LEDS);
    blocks[(size_t)block] = block;

    // WiFi
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
    server.on("/", []() {
        server.send(200, "text/plain", "hello from esp8266!");
    });
    server.on("/test", HTTPMethod::HTTP_GET, []() {
        server.send(200, "text/plain", "hahaha get");
    });
    server.on("/test", HTTPMethod::HTTP_POST, []() {
        server.send(200, "text/plain", "hahaha post");
    });

    /*
        BLOCK API
    */

    /*
        GET /blocks
        Returns all currently existing blocks in JSON serialized form.
    */
    server.on("/blocks", HTTP_GET, [](){
        // init
        DynamicJsonBuffer buff;
        JsonObject& result = buff.createObject();

        // serialize all blocks
        for (auto iter = blocks.begin(); iter != blocks.end(); iter++) {
            iter->second->serialize(result, String(iter->first));
        }

        // return
        String out;
        result.printTo(out);
        server.send(200, "application/json", out);
    });

    /*
        POST /blocks/get
        Returns the given block in JSON serialized form.
    */
    onJSON(server, "/blocks/get", HTTP_POST, [](JsonObject& root) {

        size_t blockAddr;
        JsonError* e;

        // blockID
        if (e = json_size_t(root, &blockAddr, "block")) {
            server.send(400, "text/plain", e->tostr());
            delete e;
            return;
        }

        // fetch block
        Block* b;
        if (blocks.find(blockAddr) == blocks.end()) {
            server.send(404, "text/plain", "block not found");
            return;
        }
        b = blocks[blockAddr];

        // serialize
        DynamicJsonBuffer buff;
        JsonObject& result = buff.createObject();
        b->serialize(result);

        // return result
        String out;
        result.printTo(out);
        server.send(200, "application/json", out);
    });

    /*
        EVENTS
    */

    /*
        POST /event
        Creates a blink event with the given parameters.
    */
    onJSON(server, "/event", HTTP_POST, [](JsonObject& root) {

        JsonError* e;
        EventBlink* ev;
        e = EventBlink::deserialize(root, &ev, "blink");

        if (e != NULL) {
            server.send(400, "text/plain", e->tostr());
            delete e;
            return;
        }

        // Initiate the event
        (*blocks.begin()).second->event((Event*) ev);
        server.send(200, "text/plain", "event started");
    });

    /*
        OTHER
    */

    /* 404 */
    server.onNotFound([]() {
        String message = "File Not Found\n\n";
        message += "URI: ";
        message += server.uri();
        message += "\nMethod: ";
        message += (server.method() == HTTP_GET) ? "GET" : "POST";
        message += "\nArguments: ";
        message += server.args();
        message += "\n";
        for (uint8_t i = 0; i < server.args(); i++) {
            message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
        }
        server.send(404, "text/plain", message);
    });
    server.begin();

    Serial.println("HTTP server started");
    ticker.attach_ms(20, []() {
        for (std::unordered_map<size_t, Block*>::iterator iter = blocks.begin(); iter != blocks.end(); iter++) {
            iter->second->render();
        }
        FastLED.show();
    });
}

void loop() {
    server.handleClient();
}
