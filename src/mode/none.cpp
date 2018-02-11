#include "none.h"

ModeNone::ModeNone() {
  this->_color = new RGBA(0, 255, 0, 255);
}

ModeNone::~ModeNone() {
  delete this->_color;
}

void ModeNone::configure(JsonObject &conf) {
  if (conf.containsKey("color")) {
    JsonObject &color = conf["color"];
    if (color.containsKey("r"))
      this->_color->r = conf["color"]["r"];
    if (color.containsKey("g"))
      this->_color->g = conf["color"]["g"];
    if (color.containsKey("b"))
      this->_color->b = conf["color"]["b"];
    if (color.containsKey("a"))
      this->_color->a = conf["color"]["a"];
  }
}

void ModeNone::render(Pixels *pixels) {
  for (int i = 0; i < pixels->len; i++) {
    this->_color->into_crgb(pixels->colors[i]);
  }
}
