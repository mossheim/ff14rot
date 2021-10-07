#include "jobstate.hpp"

#include <cmath>

constexpr float critMultiplier = 1.2;

Damage JobState::advanceTo(Time time)
{
    auto delta = time - currentTime_;
    currentTime_ = time;

    Damage totalDmg = 0;
    for (auto& eff : effects_) {
        if (&eff == &effects_[ACTID_DRG_ChaosThrust]) {
            auto newTime = std::max(eff - delta, 0.f);
            for (auto chaosGrid = std::ceilf(newTime / 3.f) * 3; chaosGrid < eff; chaosGrid += 3)
                totalDmg += 50;
            eff = newTime;
        } else
            eff = std::max(eff - delta, 0.f);
    }

    return totalDmg;
}

Damage JobState::processAction(const Action& action)
{
    auto result = getDamage(action, *this);
    // TODO do these stack?
    if (effects_[ACTID_DRG_Disembowel] > 0) {
        result *= 1.1;
    }
    if (effects_[ACTID_DRG_LanceCharge] > 0) {
        result *= 1.15;
    }
    if (effects_[ACTID_DRG_LifeSurge] > 0 && getIsGcd(action)) {
        result *= critMultiplier;
    }

    applyEffects(action);
    return result;
}

void JobState::applyEffects(const Action& action)
{
    inCombo_ = getCombo(action, *this);

    if (getIsGcd(action)) {
        lastGcd_ = getId(action);
        if (lastGcd_ == ACTID_DRG_Disembowel)
            effects_[ACTID_DRG_Disembowel] = 30;
        else if (lastGcd_ == ACTID_DRG_ChaosThrust && inCombo_)
            effects_[ACTID_DRG_ChaosThrust] = 24;
        effects_[ACTID_DRG_LifeSurge] = 0;
    } else if (getId(action) == ACTID_DRG_LifeSurge)
        effects_[ACTID_DRG_LifeSurge] = 5;
    else if (getId(action) == ACTID_DRG_LanceCharge)
        effects_[ACTID_DRG_LanceCharge] = 20;
}
