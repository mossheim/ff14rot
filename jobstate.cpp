#include "jobstate.hpp"

constexpr float critMultiplier = 1.2;

Damage JobState::advanceTo(Time time)
{
    auto delta = time - currentTime_;
    currentTime_ = time;
    std::vector<decltype(effects_)::iterator> toErase;
    for (auto it = effects_.begin(); it != effects_.end(); ++it) {
        it->second -= delta;
        if (it->second <= 0)
            toErase.push_back(it);
    }

    for (auto& it : toErase)
        effects_.erase(it);

    return 0;
}

Damage JobState::processAction(const Action& action)
{
    auto result = getDamage(action, *this);
    if (effects_.count(ACT_DRG_LifeSurge)) {
        result *= critMultiplier;
    }

    applyEffects(action);
    return result;
}

void JobState::applyEffects(const Action& action)
{
    if (getIsGcd(action)) {
        lastGcd_ = getName(action);
        effects_.erase(ACT_DRG_LifeSurge);
    }

    inCombo_ = getCombo(action, *this);

    if (getName(action) == ACT_DRG_LifeSurge) {
        effects_[ACT_DRG_LifeSurge] = 5;
    }
}
