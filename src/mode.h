
#ifndef _MODE_H
#define _MODE_H

#include "render.h"

class Mode {
public:
  virtual void configure(JsonObject& conf) = 0;
  virtual void render(Pixels* pixels) = 0;
};

#endif /* end of include guard: _MODE_H */
