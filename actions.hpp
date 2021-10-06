#pragma once

#include <variant>
#include <string>
#include <vector>

using Time = float;
using Damage = float;

struct Rotation;
struct RotationEntry;

namespace actions {

struct Noop {
    std::string name() const { return "Noop"; }
    bool isGcd() const { return true; }
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    Time delayTime() const { return 0.5; }
};

struct DRG_TrueThrust {
    std::string name() const { return "True Thrust [DRG]"; }
    bool isGcd() const { return true; }
    Time startTime(const Rotation& rot, Time gcdDelay) const;
    Time delayTime() const { return 0.1; /* TODO calc */ }
};

using Action = std::variant<Noop, DRG_TrueThrust>;

} // namespace actions

using actions::Action;

struct RotationEntry
{
    Action action;
    Time time;
};

struct Rotation
{
    std::vector<RotationEntry> entries;
};

#define GET_FIELD(_fn_) \
    return std::visit([](const auto& a) { return a. _fn_ (); }, action)

inline auto getName(const Action& action) { GET_FIELD(name); }

inline auto getStartTime(const Rotation& rot, const Action& action, Time gcdDelay)
{
    return std::visit([&](const auto& a) { return a.startTime(rot, gcdDelay); }, action);
}

inline auto getIsGcd(const Action& action) { GET_FIELD(isGcd); }
inline auto getDelayTime(const Action& action) { GET_FIELD(delayTime); }
