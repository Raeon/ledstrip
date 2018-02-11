#ifndef _EVENT_H
#define _EVENT_H

#include "render.h"

class Event {
public:
  virtual void configure(JsonObject& conf) = 0;
  virtual bool render(Pixels* pixels) = 0;
};

#endif /* end of include guard: _EVENT_H */
