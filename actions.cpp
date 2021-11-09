#include "actions.hpp"

#include <algorithm>

constexpr Time DELAY_TIME = 1.05;

Time findLastGcdTime(const Rotation& rot)
{
    auto rit = std::find_if(rot.entries.rbegin(), rot.entries.rend(), [](const auto& e) {
        return getIsGcd(e.action);
    });
    return rit == rot.entries.rend() ? -10000 : rit->time;
}

Time findLastCdTime(const Rotation& rot, ACTID actionId)
{
    auto rit = std::find_if(rot.entries.rbegin(), rot.entries.rend(), [&](const auto& e) {
        return getId(e.action) == actionId;
    });
    return rit == rot.entries.rend() ? -10000 : rit->time;
}

Time nextPossibleActionTime(const RotationEntry& entry)
{
    return DELAY_TIME + entry.time;
}

Time gcdStartTime(const Rotation& rot, Time gcdDelay)
{
    if (rot.entries.empty())
        return 0;
    return std::max(findLastGcdTime(rot) + gcdDelay, nextPossibleActionTime(rot.entries.back()));
}

Time cooldownStartTime(const Rotation& rot, Time cdDelay, ACTID actionId)
{
    if (rot.entries.empty())
        return 0;
    return std::max(findLastCdTime(rot, actionId) + cdDelay, nextPossibleActionTime(rot.entries.back()));
}

namespace actions {

} // namespace actions
