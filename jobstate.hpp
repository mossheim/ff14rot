#pragma once

#include <array>
#include <unordered_map>
#include <vector>

#include "types.hpp"

struct JobState {
    // id->time remaining
    using Effects = std::array<Time, ACTID_EFFECT_MAX>;

    Damage advanceTo(Time time);
    Damage processAction(const Action& action);

    void applyEffects(const Action& action);
    ACTID lastGcd() const { return lastGcd_; }
    bool inCombo() const { return inCombo_; }
    Damage damage() const { return damage_; }
    Time time() const { return currentTime_; }

    Time effectTime(ACTID action) const { return effects_[action]; }

private:
    Time currentTime_ = 0;
    Damage damage_ = 0;
    ACTID lastGcd_ = ACTID_Noop;
    bool inCombo_ = false;
    Effects effects_ {};
};

/*
template<size_t N>
struct R;
R<sizeof(JobState)> r;
*/
