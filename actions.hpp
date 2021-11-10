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

#define ACT_GNB_KeenEdge "((q)) Keen Edge [GNB]"
#define ACT_GNB_NoMercy "((F)) No Mercy [GNB]"
#define ACT_GNB_BrutalShell "((e)) Brutal Shell [GNB]"
#define ACT_GNB_SolidBarrel "((r)) Solid Barrel [GNB]"
#define ACT_GNB_BurstStrike "((f)) Burst Strike [GNB]"
#define ACT_GNB_SonicBreak "((Z)) Sonic Break [GNB]"
#define ACT_GNB_RoughDivide "((g)) Rough Divide [GNB]"
#define ACT_GNB_GnashingFang "((z)) Gnashing Fang [GNB]"
#define ACT_GNB_SavageClaw "((x)) Savage Claw [GNB]"
#define ACT_GNB_WickedTalon "((c)) Wicked Talon [GNB]"
#define ACT_GNB_BowShock "((X)) Bow Shock [GNB]"
#define ACT_GNB_Continuation "((v)) Continuation [GNB]"
#define ACT_GNB_Bloodfest "((V)) Bloodfest [GNB]"
#define ACT_GNB_BlastingZone "((C)) Blasting Zone [GNB]"

Time gcdStartTime(const Rotation& rot, Time gcdDelay);
Time gcdExtendedCooldownStartTime(const Rotation& rot, Time gcdDelay, Time cdDelay, ACTID actionId);
Time cooldownStartTime(const Rotation& rot, Time cdDelay, ACTID actionId);
Time multiCooldownStartTime(const Rotation& rot, Time cdDelay, int charges, ACTID actionId);

namespace actions {

struct Action {
    ACTID v;

    Action(ACTID id)
        : v(id)
    {
    }

    std::string_view name() const;
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    bool isGcd() const;
    Damage damage(const JobState& state) const;
    // deprecated
    bool combo(const JobState& state) const;

private:
    enum class Timing {
        Gcd,
        GcdExtended,
        Ogcd,
        MultiOgcd,
    };

    Timing timingType() const;
    Time cooldownTime() const;
    int charges() const;
    void error(const std::string& msg) const;
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
