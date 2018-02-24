
#include "progress.h"
#include <list>

ModeProgress::ModeProgress() {
    this->_states     = new ProgressState*[0];
    this->_stateCount = 0;

    this->_progress          = 255;
    this->_progressBias      = false;
    this->_progressDirection = false;
    this->_blend             = false;
}

ModeProgress::~ModeProgress() {
    if (this->_states && this->_stateCount > 0) {
        for (int i = 0; i < this->_stateCount; i++) {
            delete this->_states[i];
        }
        if (this->_stateCount > 0) {
            delete this->_states;
        }
    }
}

double ModeProgress::_getFrac() {
    return ((double) this->_progress) / (double) 255.0;
}

bool ModeProgress::_getColor(RGBA** p) {
    if (this->_stateCount == 0)
        return false;

    int progress          = this->_progress;
    ProgressState* cur    = NULL;
    ProgressState* before = NULL; // less or equal
    ProgressState* after  = NULL; // greater
    uint16_t dprev, dcur;

    for (int i = 0; i < this->_stateCount; i++) {
        cur = this->_states[i];

        // There are two main scenarios: It is (less or equal) or (greater) than [progress].
        if (cur->index <= progress) {
            // Candiate for [before]

            // 256 is out of bounds for uint8_t so it is always further than cur
            dprev = (before == NULL) ? 256 : abs(progress - before->index);
            dcur  = abs(progress - cur->index);

            // If cur is closer than dprev, use it!
            if (dcur < dprev) {
                before = cur;
            }
        } else {
            // Candidate for [after]

            // 256 is out of bounds for uint8_t so it is always further than cur
            dprev = (after == NULL) ? 256 : abs(progress - after->index);
            dcur  = abs(progress - cur->index);

            // If cur is closer than dprev, use it!
            if (dcur < dprev) {
                after = cur;
            }
        }
    }

    // It is not possible to find zero colors due to the if-statement at the
    // beginning of the function.

    // If we only have one color found, we are guaranteed to use it.
    if ((before == NULL) != (after == NULL)) {
        if (before)
            before->color->clone(p);
        else
            after->color->clone(p);
        return true;
    }

    // If we have two colors found, we need to determine which to use or blend them.
    if (this->_blend) {
        double index = progress - before->index; // current index in gradient
        double range = after->index - before->index; // the index range of the gradient

        *p = before->color->gradient_one(after->color, index / range);
    } else {

        // Check which color has the bias and use that.
        if (this->_progressBias == ProgressBias::BEFORE) {
            before->color->clone(p);
        } else {
            after->color->clone(p);
        }
    }

    return true;
}

JsonError* ModeProgress::update(JsonObject& root) {
    json_try(json_uint8_t(root, &this->_progress, "progress"));
    json_try(json_bool(root, &this->_progressBias, "progressBias"));
    json_try(json_bool(root, &this->_progressDirection, "progressDirection"));
    json_try(json_bool(root, &this->_blend, "blend"));
}

void ModeProgress::render(Pixels* pixels) {

    RGBA* color = NULL;

    if (!this->_getColor(&color)) {
        // We found no color! Turn off all pixels.
        for (int i = 0; i < pixels->len; i++) {
            pixels->colors[i].r = 0;
            pixels->colors[i].g = 0;
            pixels->colors[i].b = 0;
        }
        return;
    }

    // If we got here, that means we found a color! We need to light up
    // the pixels that are turned on (determined by progress) using this color.
    double progressFrac = ((double) this->_progress) / (double) 255;
    double indexFrac    = 0;
    
    int i, j;
    for (i = 0; i < pixels->len; i++) {
        j = pixels->len - i - 1;
        indexFrac = ((double) (this->_progressDirection ? j : i)) / (double) pixels->len;

        if (indexFrac <= progressFrac) {
            // The pixel should be turned on! Use the color.
            color->into_crgb(pixels->colors[i]);
        } else {
            // The pixel should be turned off.
            pixels->colors[i].r = 0;
            pixels->colors[i].g = 0;
            pixels->colors[i].b = 0;
        }
    }

    delete color;
}

void ModeProgress::serialize(JsonObject& root) {
    root["stateCount"] = this->_stateCount;
    JsonArray& states  = root.createNestedArray("colors");
    for (int i = 0; i < this->_stateCount; i++) {
        JsonObject& state = states.createNestedObject();
        this->_states[i]->color->serialize(state, "color");
        state["index"] = this->_states[i]->index;
    }
    root["progress"]          = this->_progress;
    root["progressBias"]      = this->_progressBias;
    root["progressDirection"] = this->_progressDirection;
    root["blend"]             = this->_blend;
}

void ModeProgress::serialize(JsonObject& parent, const String& key) {
    JsonObject& root = parent.createNestedObject(key);
    this->serialize(root);
}

JsonError* ModeProgress::deserialize(JsonObject& root, Mode** p) {
    JsonError* e;
    uint8_t progress;
    bool progressBias, progressDirection, blend;

    if ((e = json_uint8_t(root, &progress, "progress"))
        || (e = json_bool(root, &progressBias, "progressBias"))
        || (e = json_bool(root, &progressDirection, "progressDirection"))
        || (e = json_bool(root, &blend, "blend"))
        || (e = json_array(root, "states"))) {
        return e->trace("when parsing ModeProgress");
    }

    JsonArray& rawStates = root["states"];
    std::list<ProgressState*> states;

    for (auto rawVariant : rawStates) {
        if (!rawVariant.is<JsonObject>()) {
            // delete deserialized states
            for (auto iter : states) {
                delete iter;
            }

            // error out
            e = new JsonError((String&) "expected only state objects in state array");
            e->trace("when parsing ModeProgress");
            return e;
        }

        JsonObject& rawState = rawVariant.as<JsonObject>();
        ProgressState* ptr;
        RGBA* color = NULL;
        uint8_t index;

        if ((e = RGBA::deserialize(rawState, &color, "color"))
            || (e = json_uint8_t(rawState, &index, "index"))) {
            // delete color if parsed
            if (color != NULL)
                delete color;

            // delete deserialized states
            for (auto iter : states) {
                delete iter;
            }

            // error out
            e->trace("when parsing state objects in state array");
            return e->trace("when parsing ModeProgress");
        }

        ptr        = new ProgressState();
        ptr->color = color;
        ptr->index = index;
        states.push_back(ptr);
    }

    // convert list to array
    ProgressState** result = new ProgressState*[states.size()];

    int i = 0;
    for (auto iter : states) {
        result[i++] = iter;
    }

    // create the mode
    ModeProgress* mode       = new ModeProgress();
    mode->_stateCount        = states.size();
    mode->_states            = result;
    mode->_progress          = progress;
    mode->_progressBias      = progressBias;
    mode->_progressDirection = progressDirection;
    mode->_blend             = blend;
    *p                       = mode;

    return NULL;
}

JsonError* ModeProgress::deserialize(JsonObject& parent, Mode** p, const String& key) {
    if (!parent.containsKey(key)) return new JsonError("missing key: " + key);
    JsonObject& root = parent[key];
    JsonError* e     = ModeProgress::deserialize(root, p);
    if (e)
        e->trace("ModeProgress key: " + key);
    return e;
}