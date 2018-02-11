#ifndef _MODE_NONE_H
#define _MODE_NONE_H

#include "../mode.h"

class ModeNone : public Mode {
private:
  RGBA* _color;
public:
  ModeNone();
  ~ModeNone();
  void configure(JsonObject& conf);
  void render(Pixels* pixels);
};

#endif /* end of include guard: _MODE_NONE_H */
