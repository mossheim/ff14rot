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

Time gcdExtendedCooldownStartTime(const Rotation& rot, Time gcdDelay, Time cdDelay, ACTID actionId)
{
    return std::max(gcdStartTime(rot, gcdDelay), cooldownStartTime(rot, cdDelay, actionId));
}

Time cooldownStartTime(const Rotation& rot, Time cdDelay, ACTID actionId)
{
    if (rot.entries.empty())
        return 0;
    return std::max(findLastCdTime(rot, actionId) + cdDelay, nextPossibleActionTime(rot.entries.back()));
}

// For things like rough divide that have a 30s cooldown with 2 charges
Time multiCooldownStartTime(const Rotation& rot, Time cdDelay, int charges, ACTID actionId)
{
    const auto& entries = rot.entries;
    if (entries.empty())
        return 0;

    const auto nextPossTime = nextPossibleActionTime(rot.entries.back());

    auto it = std::find_if(entries.begin(), entries.end(), [&](const auto& e) {
        return getId(e.action) == actionId;
    });
    if (it == entries.end())
        return nextPossTime;

    auto nUses = std::count_if(it + 1, entries.end(), [&](const auto& e) {
        return getId(e.action) == actionId;
    });

    nUses++;

    return std::max(it->time + (nUses - charges + 1) * cdDelay, nextPossTime);
}

namespace actions {

// GNB damage calcs

Damage GNB_BrutalShell::damage(const JobState& js) const
{
    return js.effectTime(ACTID_GNB_KeenEdge) > 0 ? 300 : -1000;
}

Damage GNB_SolidBarrel::damage(const JobState& js) const
{
    return js.effectTime(ACTID_GNB_BrutalShell) > 0 ? 400 : -1000;
}

Damage GNB_BurstStrike::damage(const JobState& js) const
{
    return js.effectTime(ACTID_GNB_Cartridge1) > 0 ? 500 : -1000;
}

Damage GNB_GnashingFang::damage(const JobState& js) const
{
    return js.effectTime(ACTID_GNB_Cartridge1) > 0 ? 450 : -1000;
}

Damage GNB_SavageClaw::damage(const JobState& js) const
{
    return js.effectTime(ACTID_GNB_GnashingFang) > 0 ? 550 : -1000;
}

Damage GNB_WickedTalon::damage(const JobState& js) const
{
    return js.effectTime(ACTID_GNB_SavageClaw) > 0 ? 650 : -1000;
}

Damage GNB_Continuation::damage(const JobState& js) const
{
    if (js.effectTime(ACTID_GNB_Continuation) == 0)
        return -1000;

    if (js.effectTime(ACTID_GNB_GnashingFang) > 5)
        return 260;
    else if (js.effectTime(ACTID_GNB_SavageClaw) > 5)
        return 280;
    else if (js.effectTime(ACTID_GNB_WickedTalon) > 5)
        return 300;
    else
        return -1000;
}

} // namespace actions
