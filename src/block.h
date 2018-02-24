#ifndef _BLOCK_H
#define _BLOCK_H

#include "event.h"
#include "json.h"
#include "mode.h"
#include "render.h"
#include <cstdint>
#include <list>

class Block {
private:
    Pixels* _pixels;

    Mode* _mode;
    std::list<Event*> _events;

public:
    Block(CRGB* pixv, uint16_t pixc);
    ~Block();

    void update(JsonObject& root);
    void render();

    void mode(Mode* m);
    void event(Event* ev);

    void serialize(JsonObject& parent);
    void serialize(JsonObject& parent, const String& key);
    static JsonError* deserialize(JsonObject& parent, Block* &p, const String& key);
};

#endif /* end of include guard: _BLOCK_H */
