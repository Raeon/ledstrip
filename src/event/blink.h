#ifndef _EVENT_BLINK_H
#define _EVENT_BLINK_H

#include "../event.h"

class EventBlink : public Event {
private:
  RGBA* _color;

  uint8_t _blinkTimes; // how many times we've blinked
  uint8_t _blinkCount; // how many times we should blink

  bool _blink;
  uint16_t _blinkLength; // ticks before flipping
  uint16_t _blinkTicker; // tick counter

public:
  EventBlink();
  ~EventBlink();
  void configure(JsonObject& conf);
  bool render(Pixels* pixels);
};

#endif /* end of include guard: _EVENT_BLINK_H */
