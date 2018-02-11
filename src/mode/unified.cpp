
#include "unified.h"

ModeUnified::ModeUnified() {
  this->_colors = NULL;
  this->_colorIndex = 0;
  this->_colorCount = 0;

  this->_trans = false;
  this->_transInterval = 0;
  this->_transLength = 0;
  this->_transTicker = 0;

  // TODO: remove below this line
  this->_colors = new RGBA*[6];
  this->_colors[0] = new RGBA(255, 0, 0, 255);
  this->_colors[1] = new RGBA(255, 255, 0, 255);
  this->_colors[2] = new RGBA(0, 255, 0, 255);
  this->_colors[3] = new RGBA(0, 255, 255, 255);
  this->_colors[4] = new RGBA(0, 0, 255, 255);
  this->_colors[5] = new RGBA(255, 0, 255, 255);
  this->_colorCount = 6;

  this->_transInterval = 0;
  this->_transLength = 300;
}

ModeUnified::~ModeUnified() {
  if (this->_colors && this->_colorCount > 0) {
    for (int i = 0; i < this->_colorCount; i++) {
      delete this->_colors[i];
    }
    delete this->_colors;
  }
}

void ModeUnified::configure(JsonObject &conf) {
  // TODO
}

void ModeUnified::render(Pixels *pixels) {

  // Start transition if necessary
  this->_transTicker++;
  if (!this->_trans && this->_transTicker >= this->_transInterval && this->_transLength > 0) {
    this->_trans = true;
    this->_transTicker = 0;
  }

  // Stop transition if necessary
  if (this->_trans && this->_transTicker >= this->_transLength) {
    this->_trans = false;
    this->_colorIndex++;
    if (this->_colorIndex >= this->_colorCount)
      this->_colorIndex = 0;
    this->_transTicker = 0;
  }

  // Handle transition
  if (this->_trans) {
    RGBA* begin = this->_colors[this->_colorIndex];
    RGBA* end = this->_colors[this->_colorIndex + 1 >= this->_colorCount ? 0 : this->_colorIndex + 1];
    RGBA* current = begin->gradient_one(end, ((double)this->_transTicker) / (double)this->_transLength);

    for (int i = 0; i < pixels->len; i++)
      current->into_crgb(pixels->colors[i]);

    delete current;
    return;
  }

  // Handle normal scenario
  for (int i = 0; i < pixels->len; i++)
    this->_colors[this->_colorIndex]->into_crgb(pixels->colors[i]);
}
