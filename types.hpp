#pragma once

using Time = float;
using Damage = float;

struct Rotation;
struct RotationEntry;
struct JobState;

namespace actions {
struct Action;
}
using actions::Action;

enum ACTID {
    ACTID_DRG_ChaosThrust,
    ACTID_DRG_Disembowel,
    ACTID_DRG_LanceCharge,
    ACTID_DRG_LifeSurge,
    ACTID_DRG_BattleLitany,
    ACTID_DRG_BloodOfTheDragon,

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

    ACTID_MAX,
};