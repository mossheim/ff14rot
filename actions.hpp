#pragma once

#include <string>
#include <variant>
#include <vector>

#include "jobstate.hpp"
#include "types.hpp"

#define ACT_Noop "Noop"

#define ACT_DamageBuffPotion30 "8% Damage Buff Potion 30s"

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

#define ACT_GNB_KeenEdge "Keen Edge [GNB]"
#define ACT_GNB_NoMercy "No Mercy [GNB]"
#define ACT_GNB_BrutalShell "Brutal Shell [GNB]"
#define ACT_GNB_SolidBarrel "Solid Barrel [GNB]"
#define ACT_GNB_BurstStrike "Burst Strike [GNB]"
#define ACT_GNB_SonicBreak "Sonic Break [GNB]"
#define ACT_GNB_RoughDivide "Rough Divide [GNB]"
#define ACT_GNB_GnashingFang "Gnashing Fang [GNB]"
#define ACT_GNB_SavageClaw "Savage Claw [GNB]"
#define ACT_GNB_WickedTalon "Wicked Talon [GNB]"
#define ACT_GNB_BowShock "Bow Shock [GNB]"
#define ACT_GNB_Continuation "Continuation [GNB]"
#define ACT_GNB_Bloodfest "Bloodfest [GNB]"
#define ACT_GNB_BlastingZone "Blasting Zone [GNB]"

Time gcdStartTime(const Rotation& rot, Time gcdDelay);
Time gcdExtendedCooldownStartTime(const Rotation& rot, Time gcdDelay, Time cdDelay, ACTID actionId);
Time cooldownStartTime(const Rotation& rot, Time cdDelay, ACTID actionId);
Time multiCooldownStartTime(const Rotation& rot, Time cdDelay, int charges, ACTID actionId);

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
struct GcdExtendedCooldown {
    ACTID id() const { return ID; }
    bool isGcd() const { return true; }
    bool combo(const JobState& jobState) const { return false; }
    Time startTime(const Rotation& rot, Time gcdDelay) const
    {
        return gcdExtendedCooldownStartTime(rot, gcdDelay, CDTIME, ID);
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

template <ACTID ID, int CHARGES, int CDTIME>
struct MultiOgcd {
    ACTID id() const { return ID; }
    bool isGcd() const { return false; }
    Time startTime(const Rotation& rot, Time gcdDelay) const
    {
        return multiCooldownStartTime(rot, CDTIME, CHARGES, ID);
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
    bool combo(const JobState& jobState) const { return false; }
};

struct DRG_TrueThrust : Gcd<ACTID_DRG_TrueThrust>, FixedDmg<290> {
    std::string name() const { return ACT_DRG_TrueThrust; }
    bool combo(const JobState& jobState) const { return true; }
};

struct DRG_VorpalThrust : Gcd<ACTID_DRG_VorpalThrust>, ComboDmg<ACTID_DRG_TrueThrust, 140, 350> {
    std::string name() const { return ACT_DRG_VorpalThrust; }
};

struct DRG_LifeSurge : Ogcd<ACTID_DRG_LifeSurge, 45>, FixedDmg<0> {
    std::string name() const { return ACT_DRG_LifeSurge; }
};

struct DRG_PiercingTalon : Gcd<ACTID_DRG_PiercingTalon>, FixedDmg<170> {
    std::string name() const { return ACT_DRG_PiercingTalon; }
    bool combo(const JobState& jobState) const { return false; }
};

struct DRG_Disembowel : Gcd<ACTID_DRG_Disembowel>, ComboDmg<ACTID_DRG_TrueThrust, 150, 320> {
    std::string name() const { return ACT_DRG_Disembowel; }
};

struct DRG_FullThrust : Gcd<ACTID_DRG_FullThrust>, ComboDmg<ACTID_DRG_VorpalThrust, 100, 530> {
    std::string name() const { return ACT_DRG_FullThrust; }
};

struct DRG_LanceCharge : Ogcd<ACTID_DRG_LanceCharge, 90>, FixedDmg<0> {
    std::string name() const { return ACT_DRG_LanceCharge; }
};

struct DRG_Jump : Ogcd<ACTID_DRG_Jump, 30>, FixedDmg<310> {
    std::string name() const { return ACT_DRG_Jump; }
};

struct DRG_DoomSpike : Gcd<ACTID_DRG_DoomSpike>, FixedDmg<170> {
    std::string name() const { return ACT_DRG_DoomSpike; }
    bool combo(const JobState& jobState) const { return false; }
};

struct DRG_SpineshatterDive : Ogcd<ACTID_DRG_SpineshatterDive, 60>, FixedDmg<240> {
    std::string name() const { return ACT_DRG_SpineshatterDive; }
};

struct DRG_ChaosThrust : Gcd<ACTID_DRG_ChaosThrust>, ComboDmg<ACTID_DRG_Disembowel, 140, 330> {
    std::string name() const { return ACT_DRG_ChaosThrust; }
};

struct DRG_DragonfireDive : Ogcd<ACTID_DRG_DragonfireDive, 120>, FixedDmg<380> {
    std::string name() const { return ACT_DRG_DragonfireDive; }
};

// --- DRG 52-60

struct DRG_BattleLitany : Ogcd<ACTID_DRG_BattleLitany, 180>, FixedDmg<0> {
    std::string name() const { return ACT_DRG_BattleLitany; }
};

struct DRG_BloodOfTheDragon : Ogcd<ACTID_DRG_BloodOfTheDragon, 25>, FixedDmg<0> {
    std::string name() const { return ACT_DRG_BloodOfTheDragon; }
};

struct DRG_FangAndClaw : Gcd<ACTID_DRG_FangAndClaw>, ComboDmg<ACTID_DRG_FullThrust, -999, 380> {
    std::string name() const { return ACT_DRG_FangAndClaw; }
};

struct DRG_WheelingThrust : Gcd<ACTID_DRG_WheelingThrust>, ComboDmg<ACTID_DRG_ChaosThrust, -999, 380> {
    std::string name() const { return ACT_DRG_WheelingThrust; }
};

struct DRG_Gierskogul : Ogcd<ACTID_DRG_Gierskogul, 30>, FixedDmg<300> {
    std::string name() const { return ACT_DRG_Gierskogul; }
};

// --- GNB 1-50

struct GNB_KeenEdge : Gcd<ACTID_GNB_KeenEdge>, FixedDmg<200> {
    std::string name() const { return ACT_GNB_KeenEdge; }
    bool combo(const JobState& jobState) const { return false; }
};

struct GNB_NoMercy : Ogcd<ACTID_GNB_NoMercy, 60>, FixedDmg<0> {
    std::string name() const { return ACT_GNB_NoMercy; }
};

struct GNB_BrutalShell : Gcd<ACTID_GNB_BrutalShell> {
    std::string name() const { return ACT_GNB_BrutalShell; }
    Damage damage(const JobState&) const;
    bool combo(const JobState& jobState) const { return false; }
};

struct GNB_SolidBarrel : Gcd<ACTID_GNB_SolidBarrel> {
    std::string name() const { return ACT_GNB_SolidBarrel; }
    Damage damage(const JobState&) const;
    bool combo(const JobState& jobState) const { return false; }
};

struct GNB_BurstStrike : Gcd<ACTID_GNB_BurstStrike> {
    std::string name() const { return ACT_GNB_BurstStrike; }
    Damage damage(const JobState&) const;
    bool combo(const JobState& jobState) const { return false; }
};

// --- GNB 51-60

struct GNB_SonicBreak : GcdExtendedCooldown<ACTID_GNB_SonicBreak, 60>, FixedDmg<300> {
    std::string name() const { return ACT_GNB_SonicBreak; }
};

struct GNB_RoughDivide : MultiOgcd<ACTID_GNB_RoughDivide, 2, 30>, FixedDmg<200> {
    std::string name() const { return ACT_GNB_RoughDivide; }
};

struct GNB_GnashingFang : GcdExtendedCooldown<ACTID_GNB_GnashingFang, 30> {
    std::string name() const { return ACT_GNB_GnashingFang; }
    Damage damage(const JobState&) const;
};

struct GNB_SavageClaw : Gcd<ACTID_GNB_SavageClaw> {
    std::string name() const { return ACT_GNB_SavageClaw; }
    Damage damage(const JobState&) const;
    bool combo(const JobState& jobState) const { return false; }
};

struct GNB_WickedTalon : Gcd<ACTID_GNB_WickedTalon> {
    std::string name() const { return ACT_GNB_WickedTalon; }
    Damage damage(const JobState&) const;
    bool combo(const JobState& jobState) const { return false; }
};

// --- GNB 61-70

struct GNB_BowShock : Ogcd<ACTID_GNB_BowShock, 60>, FixedDmg<200> {
    std::string name() const { return ACT_GNB_BowShock; }
};

struct GNB_Continuation : Ogcd<ACTID_GNB_Continuation, 0> {
    std::string name() const { return ACT_GNB_Continuation; }
    Damage damage(const JobState&) const;
};

// --- GNB 71-80

struct GNB_Bloodfest : Ogcd<ACTID_GNB_Bloodfest, 90>, FixedDmg<0> {
    std::string name() const { return ACT_GNB_Bloodfest; }
};

struct GNB_BlastingZone : Ogcd<ACTID_GNB_BlastingZone, 30>, FixedDmg<800> {
    std::string name() const { return ACT_GNB_BlastingZone; }
};

// -------

struct DamageBuffPotion30 : Ogcd<ACTID_DamageBuffPotion30, 300>, FixedDmg<0> {
    std::string name() const { return ACT_DamageBuffPotion30; }
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

        GNB_KeenEdge,
        GNB_NoMercy,
        GNB_BrutalShell,
        GNB_SolidBarrel,
        GNB_BurstStrike,
        GNB_SonicBreak,
        GNB_RoughDivide,
        GNB_GnashingFang,
        GNB_SavageClaw,
        GNB_WickedTalon,
        GNB_BowShock,
        GNB_Continuation,
        GNB_Bloodfest,
        GNB_BlastingZone,

        DamageBuffPotion30,
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

inline auto getDamage(const Action& action, const JobState& state)
{
    return std::visit([&](const auto& a) { return a.damage(state); }, action.v);
}

inline auto getCombo(const Action& action, const JobState& state)
{
    return std::visit([&](const auto& a) { return a.combo(state); }, action.v);
}
