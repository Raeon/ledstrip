#ifndef _MODE_UNIFIED_H
#define _MODE_UNIFIED_H

#include "../mode.h"

class ModeUnified : public Mode {
private:
    // Colors
    RGBA** _colors;
    uint8_t _colorIndex;
    uint8_t _colorCount;

    // Transitions
    bool _trans; // are we transitioning right now?
    uint16_t _transInterval; // updates between transitions
    uint16_t _transLength; // updates that a transition takes
    uint16_t _transTicker; // keeps track of when we are

public:
    ModeUnified();
    ~ModeUnified();
    void configure(JsonObject& conf);
    void render(Pixels* pixels);

    void serialize(JsonObject& root);
    void serialize(JsonObject& parent, const String& key);
    static JsonError* deserialize(JsonObject& root, Mode** p);
    static JsonError* deserialize(JsonObject& parent, Mode** p, const String& key);
};

#endif /* end of include guard: _MODE_UNIFIED_H */
