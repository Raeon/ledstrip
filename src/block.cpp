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
  for (auto iter = this->_events.begin(); iter != this->_events.end(); iter++) {
    bool keep = (*iter)->render(this->_pixels);
    if (!keep) {
      delete *iter; // set the bytes freeeee!
      this->_events.erase(iter);
      iter--;
    }
  }
}

void Block::event(Event* ev) {
  this->_events.push_back(ev);
}
