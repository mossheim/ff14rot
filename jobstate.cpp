#include "jobstate.hpp"
#include "actions.hpp"

#include <cmath>

constexpr float critMultiplier = 1.2;

static Damage applyDot(JobState::Effects& effects, Time delta, ACTID id, Damage potency)
{
    Damage totalDmg = 0;
    Time newTime = std::max(effects[id] - delta, 0.f);
    for (auto grid = std::ceilf(newTime / 3.f) * 3; grid < effects[id]; grid += 3)
        totalDmg += potency;
    effects[id] = newTime;
    return totalDmg;
}

Damage JobState::advanceTo(Time time)
{
    auto delta = time - currentTime_;
    currentTime_ = time;

    Damage totalDmg = 0;
    totalDmg += applyDot(effects_, delta, ACTID_DRG_Disembowel, 50);
    totalDmg += applyDot(effects_, delta, ACTID_GNB_BowShock, 90);
    totalDmg += applyDot(effects_, delta, ACTID_GNB_SonicBreak, 90);

    for (int i = ACTID_DOT_MAX + 1; i < ACTID_EFFECT_MAX; ++i)
        effects_[i] = std::max(effects_[i] - delta, 0.f);

    damage_ += totalDmg;
    return totalDmg;
}

Damage JobState::processAction(const Action& action)
{
    auto result = getDamage(action, *this);
    // TODO do these stack?
    if (effects_[ACTID_DRG_Disembowel] > 0) {
        result *= 1.1;
    }
    if (effects_[ACTID_DRG_LanceCharge] > 0) {
        result *= 1.15;
    }
    if (effects_[ACTID_DRG_BattleLitany] > 0) {
        // Crits happen roughly 10% of the time TODO this is not right
        result *= ((critMultiplier - 1) * 0.1) + 1;
    }
    if (effects_[ACTID_DRG_LifeSurge] > 0 && getIsGcd(action)) {
        result *= critMultiplier;
    }
    if (effects_[ACTID_DRG_BloodOfTheDragon] > 0) {
        if (getId(action) == ACTID_DRG_Jump ||
        getId(action) == ACTID_DRG_SpineshatterDive) {
            result *= 1.3;
        }
    } else {
        if (getId(action) == ACTID_DRG_FangAndClaw ||
            getId(action) == ACTID_DRG_WheelingThrust) {
            // cannot execute these unless BotD is active
            result = -999;
        }
    }

    if (effects_[ACTID_DamageBuffPotion30]) {
        result *= 1.08;
    }
    if (effects_[ACTID_GNB_NoMercy]) {
        result *= 1.2;
    }

    applyEffects(action);
    damage_ += result;
    return result;
}

static void addGnbCartridge(JobState::Effects& effects)
{
    if (effects[ACTID_GNB_Cartridge1])
        effects[ACTID_GNB_Cartridge2] = 3600;
    else
        effects[ACTID_GNB_Cartridge1] = 3600;
}

static void useGnbCartridge(JobState::Effects& effects)
{
    if (effects[ACTID_GNB_Cartridge2])
        effects[ACTID_GNB_Cartridge2] = 0;
    else
        effects[ACTID_GNB_Cartridge1] = 0;
}

void JobState::applyEffects(const Action& action)
{
    inCombo_ = getCombo(action, *this);

    auto id = getId(action);

    if (getIsGcd(action)) {
        lastGcd_ = id;
        if (lastGcd_ == ACTID_DRG_Disembowel)
            effects_[ACTID_DRG_Disembowel] = 30;
        else if (lastGcd_ == ACTID_DRG_ChaosThrust && inCombo_)
            effects_[ACTID_DRG_ChaosThrust] = 24;
        else if ((lastGcd_ == ACTID_DRG_WheelingThrust || lastGcd_ == ACTID_DRG_FangAndClaw) && inCombo_) {
            // WT and F&C extend BotD by 10 seconds to a max of 30
            effects_[ACTID_DRG_BloodOfTheDragon] = std::max(30.f, effects_[ACTID_DRG_BloodOfTheDragon] + 10);
        } else if (id == ACTID_GNB_KeenEdge) {
            effects_[ACTID_GNB_KeenEdge] = 15;
            effects_[ACTID_GNB_BrutalShell] = effects_[ACTID_GNB_GnashingFang] = effects_[ACTID_GNB_SavageClaw] = effects_[ACTID_GNB_WickedTalon] = 0;
        } else if (id == ACTID_GNB_BrutalShell) {
            effects_[ACTID_GNB_BrutalShell] = 15;
            effects_[ACTID_GNB_KeenEdge] = effects_[ACTID_GNB_GnashingFang] = effects_[ACTID_GNB_SavageClaw] = effects_[ACTID_GNB_WickedTalon] = 0;
        } else if (id == ACTID_GNB_SolidBarrel) {
            effects_[ACTID_GNB_BrutalShell] = effects_[ACTID_GNB_KeenEdge] = effects_[ACTID_GNB_GnashingFang] = effects_[ACTID_GNB_SavageClaw] = effects_[ACTID_GNB_WickedTalon] = 0;
            addGnbCartridge(effects_);
        } else if (id == ACTID_GNB_BurstStrike) {
            useGnbCartridge(effects_);
        } else if (id == ACTID_GNB_SonicBreak) {
            effects_[ACTID_GNB_SonicBreak] = 30;
        } else if (id == ACTID_GNB_GnashingFang) {
            useGnbCartridge(effects_);
            effects_[ACTID_GNB_GnashingFang] = 15;
            effects_[ACTID_GNB_Continuation] = 10;
            effects_[ACTID_GNB_SavageClaw] = effects_[ACTID_GNB_WickedTalon] = 0;
        } else if (id == ACTID_GNB_SavageClaw) {
            effects_[ACTID_GNB_SavageClaw] = 15;
            effects_[ACTID_GNB_Continuation] = 10;
            effects_[ACTID_GNB_GnashingFang] = effects_[ACTID_GNB_WickedTalon] = 0;
        } else if (id == ACTID_GNB_WickedTalon) {
            effects_[ACTID_GNB_WickedTalon] = 15;
            effects_[ACTID_GNB_Continuation] = 10;
            effects_[ACTID_GNB_GnashingFang] = effects_[ACTID_GNB_SavageClaw] = 0;
        }
        effects_[ACTID_DRG_LifeSurge] = 0;
    } else if (id == ACTID_DRG_LifeSurge)
        effects_[ACTID_DRG_LifeSurge] = 5;
    else if (id == ACTID_DRG_LanceCharge)
        effects_[ACTID_DRG_LanceCharge] = 20;
    else if (id == ACTID_DRG_BattleLitany)
        effects_[ACTID_DRG_BattleLitany] = 20;
    else if (id == ACTID_DRG_BloodOfTheDragon)
        effects_[ACTID_DRG_BloodOfTheDragon] = 20;
    else if (id == ACTID_GNB_NoMercy)
        effects_[ACTID_GNB_NoMercy] = 20;
    else if (id == ACTID_GNB_BowShock)
        effects_[ACTID_GNB_BowShock] = 15;
    else if (id == ACTID_GNB_Continuation)
        effects_[ACTID_GNB_Continuation] = 0;
    else if (id == ACTID_GNB_Bloodfest)
        effects_[ACTID_GNB_Cartridge1] = effects_[ACTID_GNB_Cartridge2] = 3600;
    else if (id == ACTID_DamageBuffPotion30)
        effects_[ACTID_DamageBuffPotion30] = 30;
}
