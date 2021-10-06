#pragma once

#include <string>
#include <variant>
#include <vector>

using Time = float;
using Damage = float;

struct Rotation;
struct RotationEntry;
struct JobState;

#define ACT_NOOP "Noop"
#define ACT_DRG_TrueThrust "True Thrust [DRG]"
#define ACT_DRG_VorpalThrust "Vorpal Thrust [DRG]"
#define ACT_DRG_LifeSurge "Life Surge [DRG]"
#define ACT_DRG_PiercingTalon "Piercing Talon [DRG]"
#define ACT_DRG_Disembowel "Disembowel [DRG]"

namespace actions {

struct Noop {
    std::string name() const { return ACT_NOOP; }
    bool isGcd() const { return true; }
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    Time delayTime() const { return 0.5; }
    Damage damage(const JobState& jobState) const;
    bool combo(const JobState& jobState) const { return false; }
};

struct DRG_TrueThrust {
    std::string name() const { return ACT_DRG_TrueThrust; }
    bool isGcd() const { return true; }
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    Time delayTime() const { return 1; /* TODO calc */ }
    Damage damage(const JobState& jobState) const;
    bool combo(const JobState& jobState) const { return false; }
};

struct DRG_VorpalThrust {
    std::string name() const { return ACT_DRG_VorpalThrust; }
    bool isGcd() const { return true; }
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    Time delayTime() const { return 1; /* TODO calc */ }
    Damage damage(const JobState& jobState) const;
    bool combo(const JobState& jobState) const;
};

struct DRG_LifeSurge {
    std::string name() const { return ACT_DRG_LifeSurge; }
    bool isGcd() const { return false; }
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    Time delayTime() const { return 1; /* TODO calc */ }
    Damage damage(const JobState& jobState) const { return 0; }
    bool combo(const JobState& jobState) const;
};

struct DRG_PiercingTalon {
    std::string name() const { return ACT_DRG_PiercingTalon; }
    bool isGcd() const { return true; }
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    Time delayTime() const { return 1; /* TODO calc */ }
    Damage damage(const JobState& jobState) const;
    bool combo(const JobState& jobState) const { return false; }
};

struct DRG_Disembowel {
    std::string name() const { return ACT_DRG_Disembowel; }
    bool isGcd() const { return true; }
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    Time delayTime() const { return 1; /* TODO calc */ }
    Damage damage(const JobState& jobState) const;
    bool combo(const JobState& jobState) const;
};

using Action = std::variant<Noop, DRG_TrueThrust, DRG_VorpalThrust, DRG_LifeSurge, DRG_PiercingTalon>;

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
