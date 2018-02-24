#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//#include <ESP8266mDNS.h>
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#include "FastLED.h"
#include "block.h"
#include "event/blink.h"
#include "mode/unified.h"
#include "mode/progress.h"
#include "json.h"
#include <Ticker.h>

#include <unordered_map>

using namespace std;

#define COLOR_ORDER GRB
#define LED_TYPE WS2812B
#define NUM_LEDS 189
#define DATA_PIN D1

CRGB* leds = new CRGB[NUM_LEDS];

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
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i].r = 255; // default is red
        leds[i].g = 0;
        leds[i].b = 0;
    }

    // Default block
    Block* block           = new Block(leds, NUM_LEDS);
    blocks[(size_t) block] = block;

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
        POST /blocks
        Returns all currently existing blocks in JSON serialized form.
    */
    server.on("/blocks", HTTP_POST, []() {
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
        POST /block/create
        Creates a new block
    */
    onJSON(server, "/block/create", HTTP_POST, [](JsonObject& root) {

        // fetch arguments
        uint16_t index, length;
        JsonError* e;

        if ((e = json_uint16_t(root, &index, "index"))
            || (e = json_uint16_t(root, &length, "length"))) {
            server.send(400, "text/plain", e->tostr());
            delete e;
            return;
        }

        // validate the arguments
        if (index > NUM_LEDS) {
            server.send(400, "text/plain", "index out of bounds");
            return;
        }
        if (index + length > NUM_LEDS) {
            server.send(400, "text/plain", "index+length out of bounds");
            return;
        }

        // create block
        Block* b = new Block(&leds[index], length);
        blocks[(size_t)b] = b;
        // TODO: check for overlap and/or too many blocks

        // build result
        DynamicJsonBuffer buff;
        JsonObject& result = buff.createObject();
        result["block"] = (size_t)b;
        
        // return result
        String str;
        result.printTo(str);
        server.send(200, "application/json", str);
    });

    /*
        POST /block/get
        Returns the given block in JSON serialized form.
    */
    onJSON(server, "/block/get", HTTP_POST, [](JsonObject& root) {

        size_t blockAddr;
        JsonError* e;

        // block identifier
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
        POST /block/delete
        Deletes the specified block and turns off all pixels.
    */
    onJSON(server, "/block/delete", HTTP_POST, [](JsonObject& root) {
        size_t blockAddr;
        JsonError* e;

        // block identifier
        if (e = json_size_t(root, &blockAddr, "block")) {
            server.send(400, "text/plain", e->tostr());
            delete e;
            return;
        }

        // check validity
        if (blocks.find(blockAddr) == blocks.end()) {
            server.send(404, "text/plain", "block does not exist");
            return;
        }

        // fetch and delete
        Block* b = blocks[blockAddr];
        blocks.erase(blockAddr);
        delete b;

        // return success
        server.send(200, "application/json", "{\"result\": true}");
    });

    /*
        MODES
    */

    onJSON(server, "/mode/unified", HTTP_POST, [](JsonObject& root) {
        JsonError* e;
        size_t blockAddr;
        Block* block;
        Mode* mode;

        // block identifier
        if (e = json_size_t(root, &blockAddr, "block")) {
            server.send(400, "text/plain", e->tostr());
            delete e;
            return;
        }

        // check validity
        if (blocks.find(blockAddr) == blocks.end()) {
            server.send(404, "text/plain", "block does not exist");
            return;
        }
        block = blocks[blockAddr];

        // mode
        if (e = ModeUnified::deserialize(root, &mode)) {
            server.send(400, "text/plain", e->tostr());
            delete e;
            return;
        }

        // update block mode
        block->mode(mode);
        server.send(200, "text/plain", "ok");
    });

    onJSON(server, "/mode/progress", HTTP_POST, [](JsonObject& root) {
        JsonError* e;
        size_t blockAddr;
        Block* block;
        Mode* mode;

        // block identifier
        if (e = json_size_t(root, &blockAddr, "block")) {
            server.send(400, "text/plain", e->tostr());
            delete e;
            return;
        }

        // check validity
        if (blocks.find(blockAddr) == blocks.end()) {
            server.send(404, "text/plain", "block does not exist");
            return;
        }
        block = blocks[blockAddr];

        // mode
        if (e = ModeProgress::deserialize(root, &mode)) {
            server.send(400, "text/plain", e->tostr());
            delete e;
            return;
        }

        // update block mode
        block->mode(mode);
        server.send(200, "text/plain", "ok");
    });

    onJSON(server, "/mode/update", HTTP_POST, [](JsonObject& root) {
        JsonError* e;
        size_t blockAddr;
        Block* block;
        Mode* mode;

        // block identifier
        if (e = json_size_t(root, &blockAddr, "block")) {
            server.send(400, "text/plain", e->tostr());
            delete e;
            return;
        }

        // check validity
        if (blocks.find(blockAddr) == blocks.end()) {
            server.send(404, "text/plain", "block does not exist");
            return;
        }
        block = blocks[blockAddr];

        // update mode parameters
        block->update(root);
        server.send(200, "text/plain", "ok");
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
    ticker.attach_ms(25, []() {
        for (std::unordered_map<size_t, Block*>::iterator iter = blocks.begin(); iter != blocks.end(); iter++) {
            iter->second->render();
        }
        FastLED.show();
    });
}

void loop() {
    server.handleClient();
}
