#ifndef _EVENT_BLINK_H
#define _EVENT_BLINK_H

#include "../event.h"

class EventBlink : public Event {
private:
  RGBA* _color;

  uint8_t _blinkTimes; // how many times we should blink
  uint8_t _blinkCount; // how many times we have blinked

  bool _blink;
  uint16_t _blinkLength; // ticks before flipping
  uint16_t _blinkTicker; // tick counter

public:
  EventBlink(RGBA* color, uint8_t times, uint8_t length);
  ~EventBlink();
  void configure(JsonObject& conf);
  bool render(Pixels* pixels);
  const bool parallel();
};

#endif /* end of include guard: _EVENT_BLINK_H */
