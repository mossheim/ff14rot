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

Time nextPossibleActionTime(const RotationEntry& entry)
{
    return getDelayTime(entry.action) + entry.time;
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

Time DRG_TrueThrust::startTime(const Rotation& rot, Time gcdDelay) const
{
    if (rot.entries.empty())
        return 0;
    return std::max(findLastGcdTime(rot) + gcdDelay, nextPossibleActionTime(rot.entries.back()));
}

Damage DRG_TrueThrust::damage(const JobState& state) const
{
    return 290;
}

} // namespace actions
