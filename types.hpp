#pragma once

#include <cstdint>

using Time = int32_t; // Fixed point, x100
using Damage = int32_t; // Fixed point, x100

struct Rotation;
struct RotationEntry;
struct JobState;

constexpr static Time standardComboTime = 3000;

namespace actions {
struct Action;
}
using actions::Action;

enum ACTID : uint8_t {
    ACTID_DRG_Disembowel,
    ACTID_GNB_SonicBreak,
    ACTID_GNB_EnhancedSonicBreak,
    ACTID_GNB_BowShock,
    ACTID_GNB_EnhancedBowShock,

    ACTID_DOT_MAX, // TODO chaosthrust = this?

    ACTID_DRG_ChaosThrust,
    ACTID_DRG_LanceCharge,
    ACTID_DRG_LifeSurge,
    ACTID_DRG_BattleLitany,
    ACTID_DRG_BloodOfTheDragon,

    ACTID_GNB_KeenEdge,
    ACTID_GNB_BrutalShell,
    ACTID_GNB_NoMercy,
    ACTID_GNB_RoughDivide,
    ACTID_GNB_GnashingFang,
    ACTID_GNB_SavageClaw,
    ACTID_GNB_WickedTalon,
    ACTID_GNB_BurstStrike,
    ACTID_GNB_Continuation,
    ACTID_GNB_Cartridge1,
    ACTID_GNB_Cartridge2,
    ACTID_GNB_Cartridge3,

    ACTID_DamageBuffPotion30,

    ACTID_EFFECT_MAX,

    ACTID_Noop = ACTID_EFFECT_MAX,
    ACTID_DRG_TrueThrust,
    ACTID_DRG_VorpalThrust,
    ACTID_DRG_PiercingTalon,
    ACTID_DRG_FullThrust,
    ACTID_DRG_Jump,
    ACTID_DRG_DoomSpike,
    ACTID_DRG_SpineshatterDive,
    ACTID_DRG_DragonfireDive,
    ACTID_DRG_FangAndClaw,
    ACTID_DRG_WheelingThrust,
    ACTID_DRG_Gierskogul,

    ACTID_GNB_SolidBarrel,
    ACTID_GNB_BlastingZone,
    ACTID_GNB_Bloodfest,
    ACTID_GNB_DoubleDown,

    ACTID_MAX,
};
