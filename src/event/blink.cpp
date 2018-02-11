#include "blink.h"

EventBlink::EventBlink() {
  this->_color = new RGBA(255, 0, 0, 255);

  this->_blinkTimes = 0;
  this->_blinkCount = 0;

  this->_blink = true;
  this->_blinkLength = 0;
  this->_blinkTicker = 0;

  // TODO: remove after this line
  this->_blinkTimes = 4;
  this->_blinkLength = 15; // 10 * 20ms = 200ms
}

EventBlink::~EventBlink() {
  delete this->_color;
}

void EventBlink::configure(JsonObject &conf) {
  // TODO
}

bool EventBlink::render(Pixels *pixels) {

  // Update tick counter
  this->_blinkTicker++;

  // Swap state if necessary
  if (this->_blinkTicker >= this->_blinkLength) {
    this->_blinkTicker = 0;

    if (!this->_blink)
      this->_blinkCount++;

    // exit out if we reached the max
    if (this->_blinkCount >= this->_blinkTimes)
      return false;

    // otherwise, swap the state
    this->_blink = !this->_blink;
  }

  // update the pixels to either the color or OFF.
  for (int i = 0; i < pixels->len; i++) {
    if (this->_blink) {
      this->_color->into_crgb(pixels->colors[i]);
    } else {
      pixels->colors[i].r = 0;
      pixels->colors[i].g = 0;
      pixels->colors[i].b = 0;
    }
  }
  return true;
}
