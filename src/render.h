#ifndef _RENDER_H
#define _RENDER_H

#include "ArduinoJson.h"
#include "FastLED.h"
#include "json.h"

class RGBA {
public:
    union {
        uint8_t red;
        uint8_t r;
    };
    union {
        uint8_t blue;
        uint8_t b;
    };
    union {
        uint8_t green;
        uint8_t g;
    };
    union {
        uint8_t alpha;
        uint8_t a;
    };

public:
    RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    RGBA* gradient_one(RGBA* to, double frac);
    RGBA** gradient(RGBA* to, int c);
    CRGB to_crgb();
    void into_crgb(CRGB& crgb);

    void serialize(JsonObject& root);
    void serialize(JsonObject& parent, const String& key);
    static JsonError* deserialize(JsonObject& root, RGBA** p, const String& key);
};

// Pixels
typedef CRGB Pixel;
typedef struct {
    CRGB* colors;
    uint16_t len;
} Pixels;

#endif /* end of include guard: _RENDER_H */
