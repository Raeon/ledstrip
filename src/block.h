#ifndef _BLOCK_H
#define _BLOCK_H

#include <cstdint>
#include <queue>
#include "render.h"
#include "mode.h"
#include "event.h"

class Block {
private:
  Pixels* _pixels;

  Mode* _mode;
  std::queue<Event*> _events;

public:
  Block(CRGB* pixv, uint16_t pixc);

  void configure(JsonObject& conf);
  void render();

  void event(Event* ev);
};

#endif /* end of include guard: _BLOCK_H */
