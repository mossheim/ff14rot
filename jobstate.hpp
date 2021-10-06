#pragma once

#include "actions.hpp"

#include <unordered_map>
#include <vector>

struct JobState {
    Damage advanceTo(Time time);
    Damage processAction(const Action& action);

    void applyEffects(const Action& action);
    const std::string& lastGcd() const { return lastGcd_; }
    bool inCombo() const { return inCombo_; }

private:
    Time currentTime_ = 0;
    std::string lastGcd_;
    bool inCombo_ = false;
    // name->time remaining
    std::unordered_map<std::string, Time> effects_;
};
