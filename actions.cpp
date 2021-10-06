#include "actions.hpp"

#include <algorithm>

float findLastGcdTime(const Rotation& rot)
{
    auto rit = std::find_if(rot.entries.rbegin(), rot.entries.rend(), [](const auto& e) {
        return getIsGcd(e.action);
    });
    return rit == rot.entries.rend() ? -10000 : rit->time;
}

float nextPossibleActionTime(const RotationEntry& entry)
{
    return getDelayTime(entry.action) + entry.time;
}

namespace actions
{

float Noop::startTime(const Rotation& rot, float gcdDelay) const
{
    return rot.entries.size() * gcdDelay;
}

float DRG_TrueThrust::startTime(const Rotation& rot, float gcdDelay) const
{
    if (rot.entries.empty())
        return 0;
    return std::max(findLastGcdTime(rot) + gcdDelay, nextPossibleActionTime(rot.entries.back()));
}

} // namespace actions
