#include "render.h"
#include <vector>

RGBA::RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  this->r = r;
  this->g = g;
  this->b = b;
  this->a = a;
}

RGBA* RGBA::gradient_one(RGBA *to, double frac) {

  // calculate the delta
  double d_r = ((int16_t)to->r) - (int16_t)this->r;
  double d_g = ((int16_t)to->g) - (int16_t)this->g;
  double d_b = ((int16_t)to->b) - (int16_t)this->b;
  double d_a = ((int16_t)to->a) - (int16_t)this->a;

  // return this + (delta * frac)
  RGBA* result = new RGBA(
    this->r + round(frac * d_r),
    this->g + round(frac * d_g),
    this->b + round(frac * d_b),
    this->a + round(frac * d_a)
  );

  return result;
}

RGBA** RGBA::gradient(RGBA* to, int c) {
  RGBA** out = new RGBA*[c];
  double frac;
  for (int i = 0; i < c; i++) {
    frac = ((double)i) / (double)c;
    out[i] = this->gradient_one(to, frac);
  }
  return out;
}

CRGB RGBA::to_crgb() {
  double r = ((double)this->r) * (((double)this->a) / (double)255.0);
  double g = ((double)this->g) * (((double)this->a) / (double)255.0);
  double b = ((double)this->b) * (((double)this->a) / (double)255.0);
  return CRGB((int)r, (int)g, (int)b);
}

void RGBA::into_crgb(CRGB &crgb) {
  crgb.r = ((double)this->r) * (((double)this->a) / (double)255.0);
  crgb.g = ((double)this->g) * (((double)this->a) / (double)255.0);
  crgb.b = ((double)this->b) * (((double)this->a) / (double)255.0);
}
