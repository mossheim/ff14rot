#pragma once

#include "actions.hpp"

#include <unordered_map>
#include <vector>

struct JobState {
    Damage advanceTo(Time time);
    Damage processAction(const Action& action);

    void applyEffects(const Action& action);
    ACTID lastGcd() const { return lastGcd_; }
    bool inCombo() const { return inCombo_; }

private:
    Time currentTime_ = 0;
    ACTID lastGcd_ = ACTID_Noop;
    bool inCombo_ = false;
    // id->time remaining
    std::array<Time, ACTID_MAX> effects_ {};
};
