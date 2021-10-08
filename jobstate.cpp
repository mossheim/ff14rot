#include "jobstate.hpp"
#include "actions.hpp"

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

    damage_ += totalDmg;
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
    if (effects_[ACTID_DRG_BattleLitany] > 0) {
        // Crits happen roughly 10% of the time TODO this is not right
        result *= ((critMultiplier - 1) * 0.1) + 1;
    }
    if (effects_[ACTID_DRG_LifeSurge] > 0 && getIsGcd(action)) {
        result *= critMultiplier;
    }
    if (effects_[ACTID_DRG_BloodOfTheDragon] > 0) {
        if (getId(action) == ACTID_DRG_Jump ||
        getId(action) == ACTID_DRG_SpineshatterDive) {
            result *= 1.3;
        }
    } else {
        if (getId(action) == ACTID_DRG_FangAndClaw ||
            getId(action) == ACTID_DRG_WheelingThrust) {
            // cannot execute these unless BotD is active
            result = -999;
        }
    }

    applyEffects(action);
    damage_ += result;
    return result;
}

void JobState::applyEffects(const Action& action)
{
    inCombo_ = getCombo(action, *this);

    auto id = getId(action);

    if (getIsGcd(action)) {
        lastGcd_ = id;
        if (lastGcd_ == ACTID_DRG_Disembowel)
            effects_[ACTID_DRG_Disembowel] = 30;
        else if (lastGcd_ == ACTID_DRG_ChaosThrust && inCombo_)
            effects_[ACTID_DRG_ChaosThrust] = 24;
        else if ((lastGcd_ == ACTID_DRG_WheelingThrust || lastGcd_ == ACTID_DRG_FangAndClaw) && inCombo_) {
            // WT and F&C extend BotD by 10 seconds to a max of 30
            effects_[ACTID_DRG_BloodOfTheDragon] = std::max(30.f, effects_[ACTID_DRG_BloodOfTheDragon] + 10);
        }
        effects_[ACTID_DRG_LifeSurge] = 0;
    } else if (id == ACTID_DRG_LifeSurge)
        effects_[ACTID_DRG_LifeSurge] = 5;
    else if (id == ACTID_DRG_LanceCharge)
        effects_[ACTID_DRG_LanceCharge] = 20;
    else if (id == ACTID_DRG_BattleLitany)
        effects_[ACTID_DRG_BattleLitany] = 20;
    else if (id == ACTID_DRG_BloodOfTheDragon)
        effects_[ACTID_DRG_BloodOfTheDragon] = 20;
}
