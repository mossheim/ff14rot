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
        std::cout << counter++ << "\t" << time << "\t" << action.name() << std::endl;
    }
    std::cout << "\nTotal Damage: " << totalDamage << "\n";
}

bool fail(const std::string& name)
{
    throw std::runtime_error("Couldn't find matching job for " + name);
}

Action findAction(const std::string& name)
{
    for (uint8_t i = 0; i < ACTID_MAX; ++i)
        if (Action(ACTID(i)).name().find(name) != std::string::npos)
            return Action(ACTID(i));

    fail(name);
    return Action(ACTID_MAX);
}

void stripws(std::string& s)
{
    auto pred = [](char c) { return isspace(c); };
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
            std::cerr << "(" << a.name() << ", " << t << "), ";
        std::cerr << "(" << next.name() << ", " << startTime << ")" << std::endl;
        std::cerr << "duration=" << duration << "; gcdDelay=" << gcdDelay << std::endl;
    }

    JobState state;
    Damage accumDmg = 0;
    for (auto&& [action, time] : rot.entries) {
        if (verbose)
            std::cerr << "action=" << action.name() << "; time=" << time;
        accumDmg += state.advanceTo(time);
        if (verbose)
            std::cerr << "; accumAdv=" << accumDmg;
        accumDmg += state.processAction(action);
        if (verbose)
            std::cerr << "; accumProc=" << accumDmg << std::endl;
    }

    if (verbose)
        std::cerr << "action=" << next.name() << "; time=" << startTime;
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
    RotationEntry entry { ACTID_Noop };
    Rotation rot {};

    Time duration;
    std::cin >> duration;

    Time gcdDelay;
    std::cin >> gcdDelay;

    while (parseEntry(entry)) {
        rot.entries.emplace_back(std::move(entry));
    }

    std::cout << "Parsed input:" << std::endl;
    auto totalDamage = calculatePotentialDamage(rot, ACTID_Noop, duration, duration, gcdDelay, false);
    printResult(rot, totalDamage);
}
