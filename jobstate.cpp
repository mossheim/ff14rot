#include "jobstate.hpp"

Damage JobState::advanceTo(Time time)
{
    currentTime_ = time;
    return 0;
}

Damage JobState::processAction(const Action& action)
{
    auto result = getDamage(action, *this);
    applyEffects(action);
    return result;
}

void JobState::applyEffects(const Action& action)
{
    if (getIsGcd(action))
    {
        lastGcd_ = getName(action);
    }
}
