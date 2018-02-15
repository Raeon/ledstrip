#include "block.h"
#include "mode/unified.h"

Block::Block(CRGB* colors, uint16_t len) {
    this->_pixels         = new Pixels;
    this->_pixels->colors = colors;
    this->_pixels->len    = len;

    this->_mode = new ModeUnified();
}

Block::~Block() {
    // reset pixels
    CRGB* pix = this->_pixels->colors;
    for (int i = 0; i < this->_pixels->len; i++) {
        pix[i] = CRGB(0, 0, 0);
    }

    // clean up
    delete this->_pixels;
    delete this->_mode;
}

void Block::configure(JsonObject& conf) {
    this->_mode->configure(conf);
}

void Block::render() {
    // render mode
    this->_mode->render(this->_pixels);

    // render effects
    bool first = true;
    for (std::list<Event*>::iterator iter = this->_events.begin(); iter != this->_events.end(); iter++) {
        Event* ev = *iter;

        // only run if first or parallel
        if (first || ev->parallel()) {

            // if result is false remove it and go to next without setting first=false!
            // (since nothing is rendered if false is returned)
            if (!ev->render(this->_pixels)) {
                delete ev;
                this->_events.erase(iter--);
                continue;
            }

            first = false;
        }
    }
}

void Block::mode(Mode* m) {
    Serial.println("deleting old mode");
    delete this->_mode;
    Serial.println("installing new mode");
    this->_mode = m;
}

void Block::event(Event* ev) {
    this->_events.push_back(ev);
}

void Block::serialize(JsonObject& root) {
    this->_mode->serialize(root, "mode");
}

void Block::serialize(JsonObject& parent, const String& key) {
    JsonObject& root = parent.createNestedObject(key);
    this->serialize(root);
}

JsonError* Block::deserialize(JsonObject& parent, Block* &p, const String& key) {
    return NULL;
}