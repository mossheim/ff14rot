#pragma once

#include "actions.hpp"

struct JobState {
    Damage advanceTo(Time time);
    Damage processAction(const Action& action);

    void applyEffects(const Action& action);

private:
    Time currentTime_ = 0;
    std::string lastGcd_;
};
