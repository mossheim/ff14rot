#include "actions.hpp"
#include "jobstate.hpp"

#include <algorithm>

Time findLastGcdTime(const Rotation& rot)
{
    auto rit = std::find_if(rot.entries.rbegin(), rot.entries.rend(), [](const auto& e) {
        return getIsGcd(e.action);
    });
    return rit == rot.entries.rend() ? -10000 : rit->time;
}

Time findLastCdTime(const Rotation& rot, const std::string& actionName)
{
    auto rit = std::find_if(rot.entries.rbegin(), rot.entries.rend(), [&](const auto& e) {
        return getName(e.action) == actionName;
    });
    return rit == rot.entries.rend() ? -10000 : rit->time;
}

Time nextPossibleActionTime(const RotationEntry& entry)
{
    return getDelayTime(entry.action) + entry.time;
}

Time gcdStartTime(const Rotation& rot, Time gcdDelay)
{
    if (rot.entries.empty())
        return 0;
    return std::max(findLastGcdTime(rot) + gcdDelay, nextPossibleActionTime(rot.entries.back()));
}

Time cooldownStartTime(const Rotation& rot, Time cdDelay, const std::string& actionName)
{
    if (rot.entries.empty())
        return 0;
    return std::max(findLastCdTime(rot, actionName) + cdDelay, nextPossibleActionTime(rot.entries.back()));
}

namespace actions {

Time Noop::startTime(const Rotation& rot, Time gcdDelay) const
{
    return rot.entries.size() * gcdDelay;
}

Damage Noop::damage(const JobState& state) const
{
    return 0;
}

// ---

Time DRG_TrueThrust::startTime(const Rotation& rot, Time gcdDelay) const
{
    return gcdStartTime(rot, gcdDelay);
}

Damage DRG_TrueThrust::damage(const JobState& state) const
{
    return 290;
}

// ---

Time DRG_VorpalThrust::startTime(const Rotation& rot, Time gcdDelay) const
{
    return gcdStartTime(rot, gcdDelay);
}

Damage DRG_VorpalThrust::damage(const JobState& state) const
{
    return state.lastGcd() == ACT_DRG_TrueThrust ? 350 : 140;
}

bool DRG_VorpalThrust::combo(const JobState& state) const
{
    return state.lastGcd() == ACT_DRG_TrueThrust;
}

// ---

Time DRG_LifeSurge::startTime(const Rotation& rot, Time gcdDelay) const
{
    return cooldownStartTime(rot, 45, name());
}

bool DRG_LifeSurge::combo(const JobState& jobState) const { return jobState.inCombo(); }

// ---

Time DRG_PiercingTalon::startTime(const Rotation& rot, Time gcdDelay) const
{
    return gcdStartTime(rot, gcdDelay);
}

Damage DRG_PiercingTalon::damage(const JobState& state) const
{
    return 150;
}

// ---

Time DRG_Disembowel::startTime(const Rotation& rot, Time gcdDelay) const
{
    return gcdStartTime(rot, gcdDelay);
}

Damage DRG_Disembowel::damage(const JobState& state) const
{
    return state.lastGcd() == ACT_DRG_TrueThrust ? 320 : 150;
}

bool DRG_Disembowel::combo(const JobState& state) const
{
    return state.lastGcd() == ACT_DRG_TrueThrust;
}

} // namespace actions
