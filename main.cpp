#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <tuple>
#include <iomanip>

#include "actions.hpp"

using namespace std::literals;

// List of actions plus mutable action-related state
struct Job {
    std::vector<Action> actions;
};

std::optional<Job> getJob(const std::string& jobName)
{
    if (jobName == "DRG")
        return Job{{actions::DRG_TrueThrust{}}};
    else
        return {};
}

// Calculate damage up to `duration`
float calculatePotentialDamage(const Rotation& rot, const Action& next, float startTime, float duration, float gcdDelay)
{
    return 0;
}

std::optional<RotationEntry> greedyChooseNextRotationEntry(const Rotation& rot, const Job& job, float duration, float gcdDelay)
{
    RotationEntry result{Action{}, 0.0};
    float maxDamage = -1;

    for (const auto& action : job.actions)
    {
        std::cerr << "Action loop - " << getName(action) << std::endl;
        auto time = getStartTime(rot, action, gcdDelay);
        std::cerr << "Start time " << time << std::endl;
        if (time >= duration)
            continue;

        auto damage = calculatePotentialDamage(rot, action, time, duration, gcdDelay);
        std::cerr << "Damage " << damage << std::endl;
        if (damage > maxDamage)
        {
            maxDamage = damage;
            result = {action, time};
        }
    }

    std::cerr << "Max damage " << maxDamage << std::endl;
    if (maxDamage >= 0)
        return {result};
    else
        return {};
}

// Always pick the action that will maximize <total damage> / <start time of last action>
Rotation greedyOptimalRotation(const Job& job, float duration, float gcdDelay)
{
    Rotation result;

    while (auto nextEntry = greedyChooseNextRotationEntry(result, job, duration, gcdDelay))
    {
        result.entries.push_back(std::move(*nextEntry));
    }

    return result;
}

Rotation calculateOptimalRotation(const Job& job, float duration, float gcdDelay)
{
    return greedyOptimalRotation(job, duration, gcdDelay);
}

void printResult(const Rotation& rotation, float totalDamage)
{
    int counter = 0;
    std::cout << std::setprecision(2);
    for (auto&& [action, time] : rotation.entries)
    {
        std::cout << counter++ << "\t" << time << "\t" << getName(action) << std::endl;
    }
    std::cout << "\nTotal Damage: " << totalDamage << "\n";
}

int main(int argc, char** argv)
{
    std::string usage = "Usage:\n  "s + argv[0] + " <job> <duration in seconds> <GCD delay>";
    if (argc != 4)
    {
        std::cout << usage << std::endl;
        return 1;
    }

    std::string jobName = argv[1];
    float duration = std::atof(argv[2]);
    float gcdDelay = std::atof(argv[3]);

    if (auto maybeJob = getJob(jobName))
    {
        if (duration > 0 && gcdDelay > 0)
        {
            auto result = calculateOptimalRotation(*maybeJob, duration, gcdDelay);
            auto totalDamage = calculatePotentialDamage(result, Action{}, duration, duration, gcdDelay);
            printResult(result, totalDamage);
        }
        else
        {
            std::cout << "Duration and GCD delay must be > 0" << std::endl;
            return 1;
        }
    }
    else
    {
        std::cout << "No job actions found for '" << jobName << "'." << std::endl;
        return 1;
    }
}
