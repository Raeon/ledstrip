
#include "unified.h"
#include <list>

ModeUnified::ModeUnified() {
    this->_colors     = new RGBA*[0];
    this->_colorIndex = 0;
    this->_colorCount = 0;

    this->_trans         = false;
    this->_transInterval = 0;
    this->_transLength   = 0;
    this->_transTicker   = 0;

    // TODO: remove below this line
    //this->_colors     = new RGBA*[0];
    // this->_colors[0]  = new RGBA(255, 0, 0, 255);
    // this->_colors[1]  = new RGBA(255, 255, 0, 255);
    // this->_colors[2]  = new RGBA(0, 255, 0, 255);
    // this->_colors[3]  = new RGBA(0, 255, 255, 255);
    // this->_colors[4]  = new RGBA(0, 0, 255, 255);
    // this->_colors[5]  = new RGBA(255, 0, 255, 255);
    //this->_colorCount = 0;

    //this->_transInterval = 0;
    //this->_transLength   = 300;
}

ModeUnified::~ModeUnified() {
    if (this->_colors && this->_colorCount > 0) {
        for (int i = 0; i < this->_colorCount; i++) {
            delete this->_colors[i];
        }
        if (this->_colorCount > 0) {
            delete this->_colors;
        }
    }
}

JsonError* ModeUnified::update(JsonObject& conf) {
    // TODO
}

void ModeUnified::render(Pixels* pixels) {

    if (this->_colorCount == 0)
        return;

    // Start transition if necessary
    this->_transTicker++;
    if (!this->_trans && this->_transTicker >= this->_transInterval) {
        this->_trans       = true;
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
        RGBA* begin   = this->_colors[this->_colorIndex];
        RGBA* end     = this->_colors[this->_colorIndex + 1 >= this->_colorCount ? 0 : this->_colorIndex + 1];
        RGBA* current = begin->gradient_one(end, ((double) this->_transTicker) / (double) this->_transLength);

        for (int i = 0; i < pixels->len; i++)
            current->into_crgb(pixels->colors[i]);

        delete current;
        return;
    }

    // Handle normal scenario
    for (int i = 0; i < pixels->len; i++)
        this->_colors[this->_colorIndex]->into_crgb(pixels->colors[i]);
}

void ModeUnified::serialize(JsonObject& root) {
    root["colorCount"] = this->_colorCount;
    root["colorIndex"] = this->_colorIndex;
    JsonArray& colors  = root.createNestedArray("colors");
    for (int i = 0; i < this->_colorCount; i++) {
        JsonObject& color = colors.createNestedObject();
        this->_colors[i]->serialize(color);
    }

    root["transInterval"] = this->_transInterval;
    root["transLength"]   = this->_transLength;
    root["transTicker"]   = this->_transTicker;
}

void ModeUnified::serialize(JsonObject& parent, const String& key) {
    JsonObject& root = parent.createNestedObject(key);
    this->serialize(root);
}

JsonError* ModeUnified::deserialize(JsonObject& root, Mode** p) {

    JsonError* e;
    uint16_t transInterval;
    uint16_t transLength;

    if ((e = json_uint16_t(root, &transInterval, "transInterval"))
        || (e = json_uint16_t(root, &transLength, "transLength"))
        || (e = json_array(root, "colors"))) {
        return e->trace("when parsing ModeUnified");
    }

    JsonArray& rawColors = root["colors"];
    std::list<RGBA*> colors;

    for (auto rawVariant : rawColors) {
        if (!rawVariant.is<JsonObject>()) {
            // delete deserialized colors
            for (std::list<RGBA*>::iterator iter = colors.begin(); iter != colors.end(); iter++) {
                delete *iter;
            }
            // error out
            e = new JsonError((String&) "expected only color objects in color array");
            e->trace("when parsing ModeUnified");
            return e;
        }

        // in any other scenario, deserialize this color
        RGBA* ptr;
        e = RGBA::deserialize(rawVariant.as<JsonObject>(), &ptr);
        if (e) {
            // again delete serialized colors
            for (std::list<RGBA*>::iterator iter = colors.begin(); iter != colors.end(); iter++) {
                delete *iter;
            }
            // error out
            return e->trace("when parsing ModeUnified: colors");
        }

        // store in list
        colors.push_back(ptr);
    }

    // convert list to array
    RGBA** result = new RGBA*[colors.size()];
    int i = 0;
    for (std::list<RGBA*>::iterator iter = colors.begin(); iter != colors.end(); iter++) {
        result[i++] = *iter;
    }

    // create the mode
    ModeUnified* mode = new ModeUnified();
    mode->_transInterval = transInterval;
    mode->_transLength = transLength;
    mode->_colorCount = colors.size();
    mode->_colorIndex = 0;
    mode->_colors = result;
    *p = mode;

    return NULL;
}

JsonError* ModeUnified::deserialize(JsonObject& parent, Mode** p, const String& key) {
    if (!parent.containsKey(key)) return new JsonError("missing key: " + key);
    JsonObject& root = parent[key];
    JsonError* e = ModeUnified::deserialize(root, p);
    if (e)
        e->trace("ModeUnified key: " + key);
    return e;
}