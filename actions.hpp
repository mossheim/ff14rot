#pragma once

#include <string>
#include <variant>
#include <vector>

using Time = float;
using Damage = float;

struct Rotation;
struct RotationEntry;
struct JobState;

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

enum ACTID {
    ACTID_Noop,
    ACTID_DRG_TrueThrust,
    ACTID_DRG_VorpalThrust,
    ACTID_DRG_LifeSurge,
    ACTID_DRG_PiercingTalon,
    ACTID_DRG_Disembowel,
    ACTID_DRG_FullThrust,
    ACTID_DRG_LanceCharge,
    ACTID_DRG_Jump,
    ACTID_DRG_DoomSpike,
    ACTID_DRG_SpineshatterDive,
    ACTID_DRG_ChaosThrust,
    ACTID_DRG_DragonfireDive,

    ACTID_MAX,
};

namespace actions {

struct Noop {
    std::string name() const { return ACT_Noop; }
    ACTID id() const { return ACTID_Noop; }
    bool isGcd() const { return true; }
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    Time delayTime() const { return 0.5; }
    Damage damage(const JobState& jobState) const;
    bool combo(const JobState& jobState) const { return false; }
};

struct DRG_TrueThrust {
    std::string name() const { return ACT_DRG_TrueThrust; }
    ACTID id() const { return ACTID_DRG_TrueThrust; }
    bool isGcd() const { return true; }
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    Time delayTime() const { return 1; /* TODO calc */ }
    Damage damage(const JobState& jobState) const;
    bool combo(const JobState& jobState) const { return false; }
};

struct DRG_VorpalThrust {
    std::string name() const { return ACT_DRG_VorpalThrust; }
    ACTID id() const { return ACTID_DRG_VorpalThrust; }
    bool isGcd() const { return true; }
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    Time delayTime() const { return 1; /* TODO calc */ }
    Damage damage(const JobState& jobState) const;
    bool combo(const JobState& jobState) const;
};

struct DRG_LifeSurge {
    std::string name() const { return ACT_DRG_LifeSurge; }
    ACTID id() const { return ACTID_DRG_LifeSurge; }
    bool isGcd() const { return false; }
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    Time delayTime() const { return 1; /* TODO calc */ }
    Damage damage(const JobState& jobState) const { return 0; }
    bool combo(const JobState& jobState) const;
};

struct DRG_PiercingTalon {
    std::string name() const { return ACT_DRG_PiercingTalon; }
    ACTID id() const { return ACTID_DRG_PiercingTalon; }
    bool isGcd() const { return true; }
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    Time delayTime() const { return 1; /* TODO calc */ }
    Damage damage(const JobState& jobState) const;
    bool combo(const JobState& jobState) const { return false; }
};

struct DRG_Disembowel {
    std::string name() const { return ACT_DRG_Disembowel; }
    ACTID id() const { return ACTID_DRG_Disembowel; }
    bool isGcd() const { return true; }
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    Time delayTime() const { return 1; /* TODO calc */ }
    Damage damage(const JobState& jobState) const;
    bool combo(const JobState& jobState) const;
};

struct DRG_FullThrust {
    std::string name() const { return ACT_DRG_FullThrust; }
    ACTID id() const { return ACTID_DRG_FullThrust; }
    bool isGcd() const { return true; }
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    Time delayTime() const { return 1; /* TODO calc */ }
    Damage damage(const JobState& jobState) const;
    bool combo(const JobState& jobState) const;
};

struct DRG_LanceCharge {
    std::string name() const { return ACT_DRG_LanceCharge; }
    ACTID id() const { return ACTID_DRG_LanceCharge; }
    bool isGcd() const { return false; }
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    Time delayTime() const { return 1; /* TODO calc */ }
    Damage damage(const JobState& jobState) const { return 0; };
    bool combo(const JobState& jobState) const;
};

struct DRG_Jump {
    std::string name() const { return ACT_DRG_Jump; }
    ACTID id() const { return ACTID_DRG_Jump; }
    bool isGcd() const { return false; }
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    Time delayTime() const { return 1; /* TODO calc */ }
    Damage damage(const JobState& jobState) const { return 310; };
    bool combo(const JobState& jobState) const;
};

struct DRG_DoomSpike {
    std::string name() const { return ACT_DRG_DoomSpike; }
    ACTID id() const { return ACTID_DRG_DoomSpike; }
    bool isGcd() const { return true; }
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    Time delayTime() const { return 1; /* TODO calc */ }
    Damage damage(const JobState& jobState) const;
    bool combo(const JobState& jobState) const { return false; }
};

struct DRG_SpineshatterDive {
    std::string name() const { return ACT_DRG_SpineshatterDive; }
    ACTID id() const { return ACTID_DRG_SpineshatterDive; }
    bool isGcd() const { return false; }
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    Time delayTime() const { return 1; /* TODO calc */ }
    Damage damage(const JobState& jobState) const { return 240; };
    bool combo(const JobState& jobState) const;
};

struct DRG_ChaosThrust {
    std::string name() const { return ACT_DRG_ChaosThrust; }
    ACTID id() const { return ACTID_DRG_ChaosThrust; }
    bool isGcd() const { return true; }
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    Time delayTime() const { return 1; /* TODO calc */ }
    Damage damage(const JobState& jobState) const;
    bool combo(const JobState& jobState) const;
};

struct DRG_DragonfireDive {
    std::string name() const { return ACT_DRG_DragonfireDive; }
    ACTID id() const { return ACTID_DRG_DragonfireDive; }
    bool isGcd() const { return false; }
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    Time delayTime() const { return 1; /* TODO calc */ }
    Damage damage(const JobState& jobState) const { return 380; };
    bool combo(const JobState& jobState) const;
};

using Action = std::variant<Noop, DRG_TrueThrust, DRG_VorpalThrust, DRG_LifeSurge, DRG_PiercingTalon, DRG_Disembowel,
    DRG_FullThrust, DRG_LanceCharge, DRG_Jump, DRG_DoomSpike, DRG_SpineshatterDive, DRG_ChaosThrust, DRG_DragonfireDive>;

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
    return std::visit([](const auto& a) { return a._fn_(); }, action)

inline auto getName(const Action& action)
{
    GET_FIELD(name);
}
inline auto getId(const Action& action) { GET_FIELD(id); }

inline auto getStartTime(const Rotation& rot, const Action& action, Time gcdDelay)
{
    return std::visit([&](const auto& a) { return a.startTime(rot, gcdDelay); }, action);
}

inline auto getIsGcd(const Action& action) { GET_FIELD(isGcd); }
inline auto getDelayTime(const Action& action) { GET_FIELD(delayTime); }

inline auto getDamage(const Action& action, const JobState& state)
{
    return std::visit([&](const auto& a) { return a.damage(state); }, action);
}

inline auto getCombo(const Action& action, const JobState& state)
{
    return std::visit([&](const auto& a) { return a.combo(state); }, action);
}
