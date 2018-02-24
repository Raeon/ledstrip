
#ifndef _MODE_H
#define _MODE_H

#include "render.h"

class Mode {
public:
    virtual JsonError* update(JsonObject& conf) = 0;
    virtual void render(Pixels* pixels)         = 0;

    virtual void serialize(JsonObject& root);
    virtual void serialize(JsonObject& parent, const String& key);
    // static JsonError* deserialize(JsonObject& root, Mode** p);
    // static JsonError* deserialize(JsonObject& parent, Mode** p, const String& key);
};

#endif /* end of include guard: _MODE_H */
