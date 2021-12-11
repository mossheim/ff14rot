#include "actions.hpp"

#include <algorithm>
#include <iostream>

#define UNSAFE

constexpr Time DELAY_TIME = 80;

using namespace std::string_literals;

Time findLastGcdTime(const Rotation& rot)
{
    auto rit = std::find_if(rot.entries.rbegin(), rot.entries.rend(), [](const auto& e) {
        return e.action.isGcd();
    });
    return rit == rot.entries.rend() ? -1000000 : rit->time;
}

Time findLastCdTime(const Rotation& rot, ACTID actionId)
{
    auto rit = std::find_if(rot.entries.rbegin(), rot.entries.rend(), [&](const auto& e) {
        return e.action.v == actionId;
    });
    return rit == rot.entries.rend() ? -1000000 : rit->time;
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
    auto adjustedCdDelay = cdDelay * gcdDelay / 250;
    return std::max(gcdStartTime(rot, gcdDelay), cooldownStartTime(rot, adjustedCdDelay, actionId));
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
        return e.action.v == actionId;
    });
    if (it == entries.end())
        return nextPossTime;

    int nUses = std::count_if(it + 1, entries.end(), [&](const auto& e) {
        return e.action.v == actionId;
    });

    nUses++;

    return std::max(it->time + (nUses - charges + 1) * cdDelay, nextPossTime);
}

namespace actions {

std::string_view Action::name() const
{
    switch (v) {
#define CASE(_n_)     \
    case ACTID_##_n_: \
        return ACT_##_n_
        CASE(Noop);
        CASE(DamageBuffPotion30);
        CASE(DRG_TrueThrust);
        CASE(DRG_VorpalThrust);
        CASE(DRG_LifeSurge);
        CASE(DRG_PiercingTalon);
        CASE(DRG_Disembowel);
        CASE(DRG_FullThrust);
        CASE(DRG_LanceCharge);
        CASE(DRG_Jump);
        CASE(DRG_DoomSpike);
        CASE(DRG_SpineshatterDive);
        CASE(DRG_ChaosThrust);
        CASE(DRG_DragonfireDive);
        CASE(DRG_BattleLitany);
        CASE(DRG_BloodOfTheDragon);
        CASE(DRG_FangAndClaw);
        CASE(DRG_WheelingThrust);
        CASE(DRG_Gierskogul);
        CASE(GNB_KeenEdge);
        CASE(GNB_NoMercy);
        CASE(GNB_BrutalShell);
        CASE(GNB_SolidBarrel);
        CASE(GNB_BurstStrike);
        CASE(GNB_SonicBreak);
        CASE(GNB_RoughDivide);
        CASE(GNB_GnashingFang);
        CASE(GNB_SavageClaw);
        CASE(GNB_WickedTalon);
        CASE(GNB_BowShock);
        CASE(GNB_Continuation);
        CASE(GNB_Bloodfest);
        CASE(GNB_BlastingZone);
        CASE(GNB_DoubleDown);
#undef CASE
    case ACTID_DOT_MAX:
    case ACTID_GNB_Cartridge1:
    case ACTID_GNB_Cartridge2:
    case ACTID_GNB_Cartridge3:
    case ACTID_GNB_EnhancedBowShock:
    case ACTID_GNB_EnhancedSonicBreak:
    case ACTID_MAX:
        return "XXX";
    }

#ifndef UNSAFE
    error("Name: " + std::to_string(v));
#endif
    return "UNKNOWN";
}

Time Action::startTime(const Rotation& rot, Time gcdDelay) const
{
    switch (timingType()) {
    case Timing::Gcd:
        return gcdStartTime(rot, gcdDelay);
    case Timing::GcdExtended:
        return gcdExtendedCooldownStartTime(rot, gcdDelay, cooldownTime(), v);
    case Timing::Ogcd:
        return cooldownStartTime(rot, cooldownTime(), v);
    case Timing::MultiOgcd:
        return multiCooldownStartTime(rot, cooldownTime(), charges(), v);
    }

#ifndef UNSAFE
    error("StartTime: " + std::string(name()));
#endif
    return -100;
}

bool Action::isGcd() const
{
    auto type = timingType();
    return type == Timing::Gcd || type == Timing::GcdExtended;
}

Damage Action::damage(const JobState& jobState) const
{
    switch (v) {
#define FIXED(_n_, _v_) \
    case ACTID_##_n_:   \
        return _v_
        FIXED(Noop, 0);
        FIXED(DRG_TrueThrust, 29000);
        FIXED(DRG_LifeSurge, 0);
        FIXED(DRG_PiercingTalon, 0);
        FIXED(DRG_LanceCharge, 0);
        FIXED(DRG_Jump, 31000);
        FIXED(DRG_DoomSpike, 17000);
        FIXED(DRG_SpineshatterDive, 24000);
        FIXED(DRG_BloodOfTheDragon, 0);
        FIXED(DRG_DragonfireDive, 38000);
        FIXED(DRG_BattleLitany, 0);
        FIXED(DRG_Gierskogul, 30000);
        FIXED(GNB_KeenEdge, 17000);
        FIXED(GNB_NoMercy, 0);
        FIXED(GNB_SonicBreak, 30000);
        FIXED(GNB_RoughDivide, 15000);
        FIXED(GNB_BowShock, 15000);
        FIXED(GNB_Bloodfest, 0);
        FIXED(GNB_BlastingZone, 70000);
        FIXED(DamageBuffPotion30, 0);
#undef FIXED
#define COMBO(_n_, _other_, _bare_, _combodmg_) \
    case ACTID_##_n_:                           \
        return (jobState.inCombo() && jobState.lastGcd() == ACTID_##_other_) ? _combodmg_ : _bare_
        COMBO(DRG_VorpalThrust, DRG_TrueThrust, 140, 35000);
        COMBO(DRG_Disembowel, DRG_TrueThrust, 150, 32000);
        COMBO(DRG_FullThrust, DRG_VorpalThrust, 100, 53000);
        COMBO(DRG_ChaosThrust, DRG_Disembowel, 140, 33000);
        COMBO(DRG_FangAndClaw, DRG_FullThrust, -999, 38000);
        COMBO(DRG_WheelingThrust, DRG_ChaosThrust, -999, 38000);
#undef COMBO
#define GNBCOMBO(_n_, _other_, _dmg_) \
    case ACTID_##_n_:                 \
        return jobState.effectTime(ACTID_##_other_) > 0 ? _dmg_ : -100000
        GNBCOMBO(GNB_BrutalShell, GNB_KeenEdge, 26000);
        GNBCOMBO(GNB_SolidBarrel, GNB_BrutalShell, 34000);
        GNBCOMBO(GNB_BurstStrike, GNB_Cartridge1, 38000);
        GNBCOMBO(GNB_GnashingFang, GNB_Cartridge1, 36000);
        GNBCOMBO(GNB_DoubleDown, GNB_Cartridge2, 120000);
        GNBCOMBO(GNB_SavageClaw, GNB_GnashingFang, 44000);
        GNBCOMBO(GNB_WickedTalon, GNB_SavageClaw, 52000);
#undef GNBCOMBO
    case ACTID_GNB_Continuation:
        {
            if (jobState.effectTime(ACTID_GNB_Continuation) == 0)
                return -100000;

            ACTID mostRecentActId = ACTID_MAX;
            Time highestTime = 0;
            for (auto id : {ACTID_GNB_GnashingFang, ACTID_GNB_SavageClaw, ACTID_GNB_WickedTalon, ACTID_GNB_BurstStrike})
            {
                Time actTime = jobState.effectTime(id);
                if (actTime > standardComboTime - 1000 && actTime > highestTime)
                {
                    highestTime = actTime;
                    mostRecentActId = id;
                }
            }

            if (mostRecentActId == ACTID_GNB_GnashingFang)
                return 18000;
            else if (mostRecentActId == ACTID_GNB_SavageClaw)
                return 22000;
            else if (mostRecentActId == ACTID_GNB_WickedTalon)
                return 26000;
            else if (mostRecentActId == ACTID_GNB_BurstStrike)
                return 18000;
            else
                return -100000;
        }

    case ACTID_GNB_EnhancedBowShock:
    case ACTID_GNB_EnhancedSonicBreak:
    case ACTID_GNB_Cartridge1:
    case ACTID_GNB_Cartridge2:
    case ACTID_GNB_Cartridge3:
    case ACTID_DOT_MAX:
    case ACTID_MAX:
        break;
    }

#ifndef UNSAFE
    error("Damage: " + std::string(name()));
#endif
    return -1000000;
}

bool Action::combo(const JobState& jobState) const
{
    switch (timingType()) {
    case Timing::Ogcd:
    case Timing::MultiOgcd:
        return jobState.inCombo();
    case Timing::GcdExtended:
        return false;
    case Timing::Gcd:
        switch (v) {
#define COMBO(_n_, _other_) \
    case ACTID_##_n_:       \
        return jobState.inCombo() && jobState.lastGcd() == ACTID_##_other_
            COMBO(DRG_VorpalThrust, DRG_TrueThrust);
            COMBO(DRG_Disembowel, DRG_TrueThrust);
            COMBO(DRG_FullThrust, DRG_VorpalThrust);
            COMBO(DRG_ChaosThrust, DRG_Disembowel);
            COMBO(DRG_FangAndClaw, DRG_FullThrust);
            COMBO(DRG_WheelingThrust, DRG_ChaosThrust);
#undef COMBO
#define FIXED(_n_, _v_) \
    case ACTID_##_n_:   \
        return _v_
            FIXED(Noop, false);
            FIXED(DRG_TrueThrust, true);
            FIXED(DRG_PiercingTalon, false);
            FIXED(DRG_DoomSpike, false);
            FIXED(GNB_KeenEdge, false);
            FIXED(GNB_BrutalShell, false);
            FIXED(GNB_SolidBarrel, false);
            FIXED(GNB_BurstStrike, false);
            FIXED(GNB_GnashingFang, false);
            FIXED(GNB_SavageClaw, false);
            FIXED(GNB_WickedTalon, false);
            FIXED(GNB_DoubleDown, false);
#undef FIXED
        default:
#ifndef UNSAFE
            error("Combo: " + std::string(name()));
#endif
            return false;
        }
    }

#ifndef UNSAFE
    error("Combo: " + std::string(name()));
#endif
    return false;
}

Action::Timing Action::timingType() const
{
    switch (v) {
#define CASE(_n_, _type_) \
    case ACTID_##_n_:     \
        return Timing::_type_
        CASE(DRG_Disembowel, Gcd);
        CASE(GNB_SonicBreak, GcdExtended);
        CASE(GNB_BowShock, Ogcd);
        CASE(DRG_ChaosThrust, Gcd);
        CASE(DRG_LanceCharge, Ogcd);
        CASE(DRG_LifeSurge, Ogcd);
        CASE(DRG_BattleLitany, Ogcd);
        CASE(DRG_BloodOfTheDragon, Ogcd);
        CASE(GNB_KeenEdge, Gcd);
        CASE(GNB_BrutalShell, Gcd);
        CASE(GNB_NoMercy, Ogcd);
        CASE(GNB_RoughDivide, MultiOgcd);
        CASE(GNB_GnashingFang, GcdExtended);
        CASE(GNB_SavageClaw, Gcd);
        CASE(GNB_WickedTalon, Gcd);
        CASE(GNB_Continuation, Ogcd);
        CASE(DamageBuffPotion30, Ogcd);
        CASE(DRG_TrueThrust, Gcd);
        CASE(DRG_VorpalThrust, Gcd);
        CASE(DRG_PiercingTalon, Gcd);
        CASE(DRG_FullThrust, Gcd);
        CASE(DRG_Jump, Ogcd);
        CASE(DRG_DoomSpike, Gcd);
        CASE(DRG_SpineshatterDive, Ogcd);
        CASE(DRG_DragonfireDive, Ogcd);
        CASE(DRG_FangAndClaw, Gcd);
        CASE(DRG_WheelingThrust, Gcd);
        CASE(DRG_Gierskogul, Ogcd);
        CASE(GNB_SolidBarrel, Gcd);
        CASE(GNB_BurstStrike, Gcd);
        CASE(GNB_BlastingZone, Ogcd);
        CASE(GNB_Bloodfest, Ogcd);
        CASE(GNB_DoubleDown, GcdExtended);
        CASE(Noop, Gcd);
#undef CASE

    case ACTID_GNB_EnhancedSonicBreak:
    case ACTID_GNB_EnhancedBowShock:
    case ACTID_GNB_Cartridge1:
    case ACTID_GNB_Cartridge2:
    case ACTID_GNB_Cartridge3:
    case ACTID_DOT_MAX:
    case ACTID_MAX:
        break;
    }

#ifndef UNSAFE
    error("TimingType: " + std::string(name()));
#endif
    return Timing::Gcd;
}

Time Action::cooldownTime() const
{
    switch (v) {
#define CASE(_n_, _val_) \
    case ACTID_##_n_:    \
        return _val_
        CASE(GNB_SonicBreak, 6000);
        CASE(GNB_BowShock, 6000);
        CASE(DRG_LanceCharge, 9000);
        CASE(DRG_LifeSurge, 4500);
        CASE(DRG_BattleLitany, 18000);
        CASE(DRG_BloodOfTheDragon, 2500);
        CASE(GNB_NoMercy, 6000);
        CASE(GNB_RoughDivide, 3000);
        CASE(GNB_GnashingFang, 3000);
        CASE(DRG_Jump, 3000);
        CASE(DRG_SpineshatterDive, 6000);
        CASE(DRG_DragonfireDive, 12000);
        CASE(DRG_Gierskogul, 3000);
        CASE(GNB_BlastingZone, 3000);
        CASE(GNB_Bloodfest, 9000);
        CASE(GNB_Continuation, 0);
        CASE(GNB_DoubleDown, 6000);
        CASE(DamageBuffPotion30, 36000);
#undef CASE

    case ACTID_DRG_Disembowel:
    case ACTID_GNB_KeenEdge:
    case ACTID_DRG_ChaosThrust:
    case ACTID_GNB_BrutalShell:
    case ACTID_GNB_SavageClaw:
    case ACTID_GNB_WickedTalon:
    case ACTID_DRG_TrueThrust:
    case ACTID_DRG_VorpalThrust:
    case ACTID_DRG_PiercingTalon:
    case ACTID_DRG_FullThrust:
    case ACTID_DRG_DoomSpike:
    case ACTID_DRG_FangAndClaw:
    case ACTID_DRG_WheelingThrust:
    case ACTID_GNB_SolidBarrel:
    case ACTID_GNB_BurstStrike:
    case ACTID_GNB_EnhancedSonicBreak:
    case ACTID_GNB_EnhancedBowShock:
    case ACTID_GNB_Cartridge1:
    case ACTID_GNB_Cartridge2:
    case ACTID_GNB_Cartridge3:
    case ACTID_DOT_MAX:
    case ACTID_MAX:
    case ACTID_Noop:
        break;
    }

#ifndef UNSAFE
    error("CooldownTime: " + std::string(name()));
#endif
    return 0;
}

int Action::charges() const
{
#ifdef UNSAFE
    return 2;
#else
    if (v == ACTID_GNB_RoughDivide) {
        return 2;
    } else {
        error("Charges: " + std::string(name()));
        return 0;
    }
#endif
}

void Action::error(const std::string& msg) const
{
    std::cerr << "ERROR: " << msg << std::endl;
}

} // namespace actions
