#include "actions.hpp"
#include "jobstate.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>

void printResult(const Rotation& rotation, Time totalDamage)
{
    int counter = 0;
    std::cout << std::setprecision(2) << std::fixed;
    for (auto&& [action, time] : rotation.entries) {
        std::cout << counter++ << "\t" << time << "\t" << getName(action) << std::endl;
    }
    std::cout << "\nTotal Damage: " << totalDamage << "\n";
}

template <typename T>
bool check(const std::string& name, Action::Impl& impl)
{
    if (T {}.name().find(name) != std::string::npos) {
        impl = T {};
        return true;
    } else {
        return false;
    }
}

bool fail(const std::string& name)
{
    throw std::runtime_error("Couldn't find matching job for " + name);
}

template <typename... Ts>
void findActionImpl(const std::string& name, std::variant<Ts...>& action)
{
    (check<Ts>(name, action) || ...) || fail(name);
}

Action findAction(const std::string& name)
{
    Action::Impl impl;
    findActionImpl(name, impl);
    return { impl };
}

void stripws(std::string& s)
{
    auto pred = [](char c) { return c == ' '; };
    s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), pred));
    if (s.empty())
        return;

    while (s.back() == ' ')
        s.pop_back();
}

bool parseEntry(RotationEntry& entry)
{
    std::cin >> entry.time;
    std::string name;
    getline(std::cin, name);
    stripws(name);
    entry.action = findAction(name);
    return bool(std::cin);
}

Damage calculatePotentialDamage(const Rotation& rot, const Action& next, Time startTime, Time duration, Time gcdDelay, bool verbose = false)
{
    if (verbose) {
        std::cerr << "CPDmg" << std::endl;
        for (auto& [a, t] : rot.entries)
            std::cerr << "(" << getName(a) << ", " << t << "), ";
        std::cerr << "(" << getName(next) << ", " << startTime << ")" << std::endl;
        std::cerr << "duration=" << duration << "; gcdDelay=" << gcdDelay << std::endl;
    }

    JobState state;
    Damage accumDmg = 0;
    for (auto&& [action, time] : rot.entries) {
        if (verbose)
            std::cerr << "action=" << getName(action) << "; time=" << time;
        accumDmg += state.advanceTo(time);
        if (verbose)
            std::cerr << "; accumAdv=" << accumDmg;
        accumDmg += state.processAction(action);
        if (verbose)
            std::cerr << "; accumProc=" << accumDmg << std::endl;
    }

    if (verbose)
        std::cerr << "action=" << getName(next) << "; time=" << startTime;
    accumDmg += state.advanceTo(startTime);
    if (verbose)
        std::cerr << "; accumAdv=" << accumDmg;
    accumDmg += state.processAction(next);
    if (verbose) {
        std::cerr << "; accumProc=" << accumDmg << std::endl;
        std::cerr << "; time=" << duration;
    }
    accumDmg += state.advanceTo(duration);
    if (verbose) {
        std::cerr << "; accumAdv=" << accumDmg << std::endl;
        std::cerr << "Result=" << accumDmg << std::endl;
    }

    return accumDmg;
}

int main()
{
    RotationEntry entry { { actions::Noop {} } };
    Rotation rot {};

    Time duration;
    std::cin >> duration;

    Time gcdDelay;
    std::cin >> gcdDelay;

    while (parseEntry(entry)) {
        rot.entries.emplace_back(std::move(entry));
    }

    std::cout << "Parsed input:" << std::endl;
    auto totalDamage = calculatePotentialDamage(rot, { actions::Noop {} }, duration, duration, gcdDelay, false);
    printResult(rot, totalDamage);
}
