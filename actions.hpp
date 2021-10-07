#pragma once

#include <string>
#include <variant>
#include <vector>

#include "jobstate.hpp"
#include "types.hpp"

#define ACT_Noop "Noop"
#define ACT_DRG_TrueThrust "True Thrust [DRG]"
#define ACT_DRG_VorpalThrust "Vorpal Thrust [DRG]"
#define ACT_DRG_LifeSurge "Life Surge [DRG]"
#define ACT_DRG_PiercingTalon "Piercing Talon [DRG]"
#define ACT_DRG_Disembowel "Disembowel [DRG]"
#define ACT_DRG_FullThrust "Full Thrust [DRG]"
#define ACT_DRG_LanceCharge "Lance Charge [DRG]"
#define ACT_DRG_Jump "Jump [DRG]"
#define ACT_DRG_DoomSpike "Doom Spike [DRG]"
#define ACT_DRG_SpineshatterDive "Spineshatter Dive [DRG]"
#define ACT_DRG_ChaosThrust "Chaos Thrust [DRG]"
#define ACT_DRG_DragonfireDive "Dragonfire Dive [DRG]"
#define ACT_DRG_BattleLitany "Battle Litany [DRG]"
#define ACT_DRG_BloodOfTheDragon "Blood of the Dragon [DRG]"
#define ACT_DRG_FangAndClaw "Fang and Claw [DRG]"
#define ACT_DRG_WheelingThrust "Wheeling Thrust [DRG]"
#define ACT_DRG_Gierskogul "Gierskogul [DRG]"

Time gcdStartTime(const Rotation& rot, Time gcdDelay);
Time cooldownStartTime(const Rotation& rot, Time cdDelay, ACTID actionId);

namespace actions {

template <ACTID ID>
struct Gcd {
    ACTID id() const { return ID; }
    bool isGcd() const { return true; }
    Time startTime(const Rotation& rot, Time gcdDelay) const
    {
        return gcdStartTime(rot, gcdDelay);
    }
};

template <ACTID ID, int CDTIME>
struct Ogcd {
    ACTID id() const { return ID; }
    bool isGcd() const { return false; }
    Time startTime(const Rotation& rot, Time gcdDelay) const
    {
        return cooldownStartTime(rot, CDTIME, ID);
    }
    bool combo(const JobState& jobState) const { return jobState.inCombo(); }
};

template <int DMG>
struct FixedDmg {
    Damage damage(const JobState&) const { return DMG; }
};

template <ACTID ID, int DMG_BARE, int DMG_COMBO>
struct ComboDmg {
    bool combo(const JobState& jobState) const
    {
        return jobState.inCombo() && jobState.lastGcd() == ID;
    }
    Damage damage(const JobState& state) const
    {
        return combo(state) ? DMG_COMBO : DMG_BARE;
    }
};

// ------

struct Noop : Gcd<ACTID_Noop>, FixedDmg<0> {
    std::string name() const { return ACT_Noop; }
    Time delayTime() const { return 0.5; }
    bool combo(const JobState& jobState) const { return false; }
};

struct DRG_TrueThrust : Gcd<ACTID_DRG_TrueThrust>, FixedDmg<290> {
    std::string name() const { return ACT_DRG_TrueThrust; }
    Time delayTime() const { return 1; /* TODO calc */ }
    bool combo(const JobState& jobState) const { return true; }
};

struct DRG_VorpalThrust : Gcd<ACTID_DRG_VorpalThrust>, ComboDmg<ACTID_DRG_TrueThrust, 140, 350> {
    std::string name() const { return ACT_DRG_VorpalThrust; }
    Time delayTime() const { return 1; /* TODO calc */ }
};

struct DRG_LifeSurge : Ogcd<ACTID_DRG_LifeSurge, 45>, FixedDmg<0> {
    std::string name() const { return ACT_DRG_LifeSurge; }
    Time delayTime() const { return 1; /* TODO calc */ }
};

struct DRG_PiercingTalon : Gcd<ACTID_DRG_PiercingTalon>, FixedDmg<170> {
    std::string name() const { return ACT_DRG_PiercingTalon; }
    Time delayTime() const { return 1; /* TODO calc */ }
    bool combo(const JobState& jobState) const { return false; }
};

struct DRG_Disembowel : Gcd<ACTID_DRG_Disembowel>, ComboDmg<ACTID_DRG_TrueThrust, 150, 320> {
    std::string name() const { return ACT_DRG_Disembowel; }
    Time delayTime() const { return 1; /* TODO calc */ }
};

struct DRG_FullThrust : Gcd<ACTID_DRG_FullThrust>, ComboDmg<ACTID_DRG_VorpalThrust, 100, 530> {
    std::string name() const { return ACT_DRG_FullThrust; }
    Time delayTime() const { return 1; /* TODO calc */ }
};

struct DRG_LanceCharge : Ogcd<ACTID_DRG_LanceCharge, 90>, FixedDmg<0> {
    std::string name() const { return ACT_DRG_LanceCharge; }
    Time delayTime() const { return 1; /* TODO calc */ }
};

struct DRG_Jump : Ogcd<ACTID_DRG_Jump, 30>, FixedDmg<310> {
    std::string name() const { return ACT_DRG_Jump; }
    Time delayTime() const { return 1; /* TODO calc */ }
};

struct DRG_DoomSpike : Gcd<ACTID_DRG_DoomSpike>, FixedDmg<170> {
    std::string name() const { return ACT_DRG_DoomSpike; }
    Time delayTime() const { return 1; /* TODO calc */ }
    bool combo(const JobState& jobState) const { return false; }
};

struct DRG_SpineshatterDive : Ogcd<ACTID_DRG_SpineshatterDive, 60>, FixedDmg<240> {
    std::string name() const { return ACT_DRG_SpineshatterDive; }
    Time delayTime() const { return 1; /* TODO calc */ }
};

struct DRG_ChaosThrust : Gcd<ACTID_DRG_ChaosThrust>, ComboDmg<ACTID_DRG_Disembowel, 140, 330> {
    std::string name() const { return ACT_DRG_ChaosThrust; }
    Time delayTime() const { return 1; /* TODO calc */ }
};

struct DRG_DragonfireDive : Ogcd<ACTID_DRG_DragonfireDive, 120>, FixedDmg<380> {
    std::string name() const { return ACT_DRG_DragonfireDive; }
    Time delayTime() const { return 1; /* TODO calc */ }
};

// --- DRG 52-60

struct DRG_BattleLitany : Ogcd<ACTID_DRG_BattleLitany, 180>, FixedDmg<0> {
    std::string name() const { return ACT_DRG_BattleLitany; }
    Time delayTime() const { return 1; /* TODO calc */ }
};

struct DRG_BloodOfTheDragon : Ogcd<ACTID_DRG_BloodOfTheDragon, 25>, FixedDmg<0> {
    std::string name() const { return ACT_DRG_BloodOfTheDragon; }
    Time delayTime() const { return 1; /* TODO calc */ }
};

struct DRG_FangAndClaw : Gcd<ACTID_DRG_FangAndClaw>, ComboDmg<ACTID_DRG_FullThrust, -999, 380> {
    std::string name() const { return ACT_DRG_FangAndClaw; }
    Time delayTime() const { return 1; /* TODO calc */ }
};

struct DRG_WheelingThrust : Gcd<ACTID_DRG_WheelingThrust>, ComboDmg<ACTID_DRG_ChaosThrust, -999, 380> {
    std::string name() const { return ACT_DRG_WheelingThrust; }
    Time delayTime() const { return 1; /* TODO calc */ }
};

struct DRG_Gierskogul : Ogcd<ACTID_DRG_Gierskogul, 30>, FixedDmg<300> {
    std::string name() const { return ACT_DRG_Gierskogul; }
    Time delayTime() const { return 1; /* TODO calc */ }
};

// -------

struct Action {
    using Impl = std::variant<
        DRG_TrueThrust,
        DRG_VorpalThrust,
        DRG_LifeSurge,
        DRG_PiercingTalon,
        DRG_Disembowel,
        DRG_FullThrust,
        DRG_LanceCharge,
        DRG_Jump,
        DRG_DoomSpike,
        DRG_SpineshatterDive,
        DRG_ChaosThrust,
        DRG_DragonfireDive,
        DRG_BattleLitany,
        DRG_BloodOfTheDragon,
        DRG_WheelingThrust,
        DRG_FangAndClaw,
        DRG_Gierskogul,
        Noop>;
    Impl v;

    Action(const Impl& impl)
        : v(impl)
    {
    }
};

} // namespace actions

using actions::Action;

struct RotationEntry {
    Action action;
    Time time;
};

struct Rotation {
    std::vector<RotationEntry> entries;
};

#define GET_FIELD(_fn_) \
    return std::visit([](const auto& a) { return a._fn_(); }, action.v)

inline auto getName(const Action& action)
{
    GET_FIELD(name);
}
inline auto getId(const Action& action) { GET_FIELD(id); }

inline auto getStartTime(const Rotation& rot, const Action& action, Time gcdDelay)
{
    return std::visit([&](const auto& a) { return a.startTime(rot, gcdDelay); }, action.v);
}

inline auto getIsGcd(const Action& action) { GET_FIELD(isGcd); }
inline auto getDelayTime(const Action& action) { GET_FIELD(delayTime); }

inline auto getDamage(const Action& action, const JobState& state)
{
    return std::visit([&](const auto& a) { return a.damage(state); }, action.v);
}

inline auto getCombo(const Action& action, const JobState& state)
{
    return std::visit([&](const auto& a) { return a.combo(state); }, action.v);
}
