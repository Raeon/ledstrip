#include "block.h"
#include "mode/unified.h"

Block::Block(CRGB *colors, uint16_t len) {
  this->_pixels = new Pixels;
  this->_pixels->colors = colors;
  this->_pixels->len = len;

  this->_mode = new ModeUnified();
}

void Block::configure(JsonObject& conf) {
  this->_mode->configure(conf);
}

void Block::render() {
  // render mode
  this->_mode->render(this->_pixels);

  // render effects
  if (this->_events.size() > 0) {
    Event* ev = this->_events.front();
    if (!ev->render(this->_pixels)) {
      delete ev;
      this->_events.pop();
    }
  }
}

void Block::event(Event* ev) {
  this->_events.push(ev);
}
