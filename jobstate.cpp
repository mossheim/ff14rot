#include "jobstate.hpp"
#include "actions.hpp"

#include <cmath>

constexpr float critMultiplier = 1.2;

static Damage applyDot(JobState::Effects& effects, Time delta, ACTID id, Damage potency)
{
    Damage totalDmg = 0;
    Time newTime = std::max(effects[id] - delta, 0);
    for (auto grid = (newTime + 299) / 300 * 300; grid < effects[id]; grid += 300)
        totalDmg += potency;
    effects[id] = newTime;
    return totalDmg;
}

Damage JobState::advanceTo(Time time)
{
    auto delta = time - currentTime_;
    currentTime_ = time;

    Damage totalDmg = 0;
    totalDmg += applyDot(effects_, delta, ACTID_DRG_Disembowel, 5000);
    totalDmg += applyDot(effects_, delta, ACTID_GNB_BowShock, 6000);
    totalDmg += applyDot(effects_, delta, ACTID_GNB_EnhancedBowShock, 60 * 120);
    totalDmg += applyDot(effects_, delta, ACTID_GNB_SonicBreak, 6000);
    totalDmg += applyDot(effects_, delta, ACTID_GNB_EnhancedSonicBreak, 60 * 120);

    for (int i = ACTID_DOT_MAX + 1; i < ACTID_EFFECT_MAX; ++i)
        effects_[i] = std::max(effects_[i] - delta, 0);

    damage_ += totalDmg;
    return totalDmg;
}

Damage JobState::processAction(const Action& action)
{
    auto result = action.damage(*this);
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
    if (effects_[ACTID_DRG_LifeSurge] > 0 && action.isGcd()) {
        result *= critMultiplier;
    }
    if (effects_[ACTID_DRG_BloodOfTheDragon] > 0) {
        if (action.v == ACTID_DRG_Jump || action.v == ACTID_DRG_SpineshatterDive) {
            result *= 1.3;
        }
    } else {
        if (action.v == ACTID_DRG_FangAndClaw || action.v == ACTID_DRG_WheelingThrust) {
            // cannot execute these unless BotD is active
            result = -100000;
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
    if (effects[ACTID_GNB_Cartridge2])
        effects[ACTID_GNB_Cartridge3] = 360000;
    else if (effects[ACTID_GNB_Cartridge1])
        effects[ACTID_GNB_Cartridge2] = 360000;
    else
        effects[ACTID_GNB_Cartridge1] = 360000;
}

static void useGnbCartridge(JobState::Effects& effects)
{
    if (effects[ACTID_GNB_Cartridge3])
        effects[ACTID_GNB_Cartridge3] = 0;
    else if (effects[ACTID_GNB_Cartridge2])
        effects[ACTID_GNB_Cartridge2] = 0;
    else
        effects[ACTID_GNB_Cartridge1] = 0;
}

void JobState::applyEffects(const Action& action)
{
    inCombo_ = action.combo(*this);

    auto id = action.v;

    if (action.isGcd()) {
        lastGcd_ = id;
        if (lastGcd_ == ACTID_DRG_Disembowel)
            effects_[ACTID_DRG_Disembowel] = 3000;
        else if (lastGcd_ == ACTID_DRG_ChaosThrust && inCombo_)
            effects_[ACTID_DRG_ChaosThrust] = 2400;
        else if ((lastGcd_ == ACTID_DRG_WheelingThrust || lastGcd_ == ACTID_DRG_FangAndClaw) && inCombo_) {
            // WT and F&C extend BotD by 10 seconds to a max of 30
            effects_[ACTID_DRG_BloodOfTheDragon] = std::max(3000, effects_[ACTID_DRG_BloodOfTheDragon] + 1000);
        } else if (id == ACTID_GNB_KeenEdge) {
            effects_[ACTID_GNB_KeenEdge] = standardComboTime;
            effects_[ACTID_GNB_BrutalShell] = effects_[ACTID_GNB_GnashingFang] = effects_[ACTID_GNB_SavageClaw] = effects_[ACTID_GNB_WickedTalon] = effects_[ACTID_GNB_BurstStrike] = effects_[ACTID_GNB_Continuation] = 0;
        } else if (id == ACTID_GNB_BrutalShell) {
            effects_[ACTID_GNB_BrutalShell] = standardComboTime;
            effects_[ACTID_GNB_KeenEdge] = effects_[ACTID_GNB_GnashingFang] = effects_[ACTID_GNB_SavageClaw] = effects_[ACTID_GNB_WickedTalon] = effects_[ACTID_GNB_BurstStrike] = effects_[ACTID_GNB_Continuation] = 0;
        } else if (id == ACTID_GNB_SolidBarrel) {
            effects_[ACTID_GNB_BrutalShell] = effects_[ACTID_GNB_KeenEdge] = effects_[ACTID_GNB_GnashingFang] = effects_[ACTID_GNB_SavageClaw] = effects_[ACTID_GNB_WickedTalon] = effects_[ACTID_GNB_BurstStrike] = effects_[ACTID_GNB_Continuation] = 0;
            addGnbCartridge(effects_);
        } else if (id == ACTID_GNB_BurstStrike) {
            useGnbCartridge(effects_);
            effects_[ACTID_GNB_BurstStrike] = standardComboTime;
            effects_[ACTID_GNB_Continuation] = 1000;
        } else if (id == ACTID_GNB_SonicBreak) {
            if (effects_[ACTID_GNB_NoMercy])
                effects_[ACTID_GNB_EnhancedSonicBreak] = 3000;
            else
                effects_[ACTID_GNB_SonicBreak] = 3000;
            effects_[ACTID_GNB_Continuation] = 0;
        } else if (id == ACTID_GNB_GnashingFang) {
            useGnbCartridge(effects_);
            effects_[ACTID_GNB_GnashingFang] = standardComboTime;
            effects_[ACTID_GNB_Continuation] = 1000;
            effects_[ACTID_GNB_SavageClaw] = effects_[ACTID_GNB_WickedTalon] = 0;
        } else if (id == ACTID_GNB_SavageClaw) {
            effects_[ACTID_GNB_SavageClaw] = standardComboTime;
            effects_[ACTID_GNB_Continuation] = 1000;
            effects_[ACTID_GNB_GnashingFang] = effects_[ACTID_GNB_WickedTalon] = 0;
        } else if (id == ACTID_GNB_WickedTalon) {
            effects_[ACTID_GNB_WickedTalon] = standardComboTime;
            effects_[ACTID_GNB_Continuation] = 1000;
            effects_[ACTID_GNB_GnashingFang] = effects_[ACTID_GNB_SavageClaw] = 0;
        } else if (id == ACTID_GNB_DoubleDown) {
            useGnbCartridge(effects_);
            useGnbCartridge(effects_);
            effects_[ACTID_GNB_Continuation] = 0;
        }
        effects_[ACTID_DRG_LifeSurge] = 0;
    } else if (id == ACTID_DRG_LifeSurge)
        effects_[ACTID_DRG_LifeSurge] = 500;
    else if (id == ACTID_DRG_LanceCharge)
        effects_[ACTID_DRG_LanceCharge] = 2000;
    else if (id == ACTID_DRG_BattleLitany)
        effects_[ACTID_DRG_BattleLitany] = 2000;
    else if (id == ACTID_DRG_BloodOfTheDragon)
        effects_[ACTID_DRG_BloodOfTheDragon] = 2000;
    else if (id == ACTID_GNB_NoMercy)
        effects_[ACTID_GNB_NoMercy] = 2000;
    else if (id == ACTID_GNB_BowShock)
        if (effects_[ACTID_GNB_NoMercy])
            effects_[ACTID_GNB_EnhancedBowShock] = 1500;
        else
            effects_[ACTID_GNB_BowShock] = 1500;
    else if (id == ACTID_GNB_Continuation)
        effects_[ACTID_GNB_Continuation] = 0;
    else if (id == ACTID_GNB_Bloodfest)
        effects_[ACTID_GNB_Cartridge1] = effects_[ACTID_GNB_Cartridge2] = effects_[ACTID_GNB_Cartridge3] = 360000;
    else if (id == ACTID_DamageBuffPotion30)
        effects_[ACTID_DamageBuffPotion30] = 3000;
}
