#ifndef _MODE_PROGRESS_H
#define _MODE_PROGRESS_H

#include "../mode.h"

typedef struct {
    RGBA* color;
    uint8_t index;
} ProgressState;

enum ProgressBias : bool {
    BEFORE = false,
    AFTER = true
};

class ModeProgress : public Mode {
private:
    // States
    ProgressState** _states;
    uint8_t _stateCount;
    
    // Progress
    uint8_t _progress;
    bool _progressBias; // determines which color to pick if no blend is active
    bool _progressDirection; // determines if the progress bar starts at the first or last pixel

    // Blending
    bool _blend;

    double _getFrac();
    bool _getColor(RGBA** p);

public:
    ModeProgress();
    ~ModeProgress();

    JsonError* update(JsonObject& conf);
    void render(Pixels* pixels);

    void serialize(JsonObject& root);
    void serialize(JsonObject& parent, const String& key);
    static JsonError* deserialize(JsonObject& root, Mode** p);
    static JsonError* deserialize(JsonObject& parent, Mode** p, const String& key);
};

#endif