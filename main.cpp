#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

#include "actions.hpp"
#include "jobstate.hpp"

using namespace std::literals;

// List of actions plus mutable action-related state
struct Job {
    std::vector<Action> actions;
};

std::optional<Job> getJob(const std::string& jobName)
{
    if (jobName == "DRG")
        return Job { { actions::DRG_TrueThrust {}, actions::DRG_VorpalThrust {}, actions::DRG_LifeSurge {} } };
    else
        return {};
}

// Calculate damage up to `duration`
Damage calculatePotentialDamage(const Rotation& rot, const Action& next, Time startTime, Time duration, Time gcdDelay)
{
    JobState state;
    Damage accumDmg = 0;
    for (auto&& [action, time] : rot.entries) {
        accumDmg += state.advanceTo(time);
        accumDmg += state.processAction(action);
    }

    accumDmg += state.advanceTo(startTime);
    accumDmg += state.processAction(next);
    accumDmg += state.advanceTo(duration);

    return accumDmg;
}

std::optional<RotationEntry> greedyChooseNextRotationEntry(const Rotation& rot, const Job& job, Time duration, Time gcdDelay)
{
    RotationEntry result { Action {}, 0.0 };
    Damage maxDamage = -1;

    for (const auto& action : job.actions) {
        std::cerr << "Action loop - " << getName(action) << std::endl;
        auto time = getStartTime(rot, action, gcdDelay);
        std::cerr << "Start time " << time << std::endl;
        if (time >= duration)
            continue;

        auto damage = calculatePotentialDamage(rot, action, time, duration, gcdDelay);
        std::cerr << "Damage " << damage << std::endl;
        if (damage > maxDamage) {
            maxDamage = damage;
            result = { action, time };
        }
    }

    std::cerr << "Max damage " << maxDamage << std::endl;
    if (maxDamage >= 0)
        return { result };
    else
        return {};
}

// Always pick the action that will maximize <total damage> / <start time of last action>
Rotation greedyOptimalRotation(const Job& job, Time duration, Time gcdDelay)
{
    Rotation result;

    while (auto nextEntry = greedyChooseNextRotationEntry(result, job, duration, gcdDelay)) {
        result.entries.push_back(std::move(*nextEntry));
    }

    return result;
}

Rotation calculateOptimalRotation(const Job& job, Time duration, Time gcdDelay)
{
    return greedyOptimalRotation(job, duration, gcdDelay);
}

void printResult(const Rotation& rotation, Time totalDamage)
{
    int counter = 0;
    std::cout << std::setprecision(2) << std::fixed;
    for (auto&& [action, time] : rotation.entries) {
        std::cout << counter++ << "\t" << time << "\t" << getName(action) << std::endl;
    }
    std::cout << "\nTotal Damage: " << totalDamage << "\n";
}

int main(int argc, char** argv)
{
    std::string usage = "Usage:\n  "s + argv[0] + " <job> <duration in seconds> <GCD delay>";
    if (argc != 4) {
        std::cout << usage << std::endl;
        return 1;
    }

    std::string jobName = argv[1];
    Time duration = std::atof(argv[2]);
    Time gcdDelay = std::atof(argv[3]);

    if (auto maybeJob = getJob(jobName)) {
        if (duration > 0 && gcdDelay > 0) {
            auto result = calculateOptimalRotation(*maybeJob, duration, gcdDelay);
            auto totalDamage = calculatePotentialDamage(result, Action {}, duration, duration, gcdDelay);
            printResult(result, totalDamage);
        } else {
            std::cout << "Duration and GCD delay must be > 0" << std::endl;
            return 1;
        }
    } else {
        std::cout << "No job actions found for '" << jobName << "'." << std::endl;
        return 1;
    }
}
