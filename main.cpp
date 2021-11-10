#include <iomanip>
#include <iostream>
#include <optional>
#include <queue>
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
    if (jobName == "DRG50")
        return Job { {
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
        } };
    else if (jobName == "DRG60")
        return Job { {
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
            ACTID_DRG_BattleLitany,
            ACTID_DRG_BloodOfTheDragon,
            ACTID_DRG_WheelingThrust,
            ACTID_DRG_FangAndClaw,
            ACTID_DRG_Gierskogul,
        } };
    else if (jobName == "GNB80_Single")
        return Job { {
            ACTID_DamageBuffPotion30,
            ACTID_GNB_KeenEdge,
            ACTID_GNB_NoMercy,
            ACTID_GNB_BrutalShell,
            ACTID_GNB_SolidBarrel,
            ACTID_GNB_BurstStrike,
            ACTID_GNB_SonicBreak,
            ACTID_GNB_RoughDivide,
            ACTID_GNB_GnashingFang,
            ACTID_GNB_SavageClaw,
            ACTID_GNB_WickedTalon,
            ACTID_GNB_BowShock,
            ACTID_GNB_Continuation,
            ACTID_GNB_Bloodfest,
            ACTID_GNB_BlastingZone,
        } };
    else
        return {};
}

// Calculate damage up to `duration`
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

auto calculatePotentialDamageIncremental(const Action& next, Time startTime, Time duration, Time gcdDelay, JobState state)
{
    state.advanceTo(startTime);
    state.processAction(next);
    auto resultState = state;
    state.advanceTo(duration);
    return std::make_pair(state.damage(), resultState);
}

std::optional<RotationEntry> greedyChooseNextRotationEntry(const Rotation& rot, const Job& job, Time duration, Time gcdDelay)
{
    RotationEntry result { ACTID_Noop, 0.0 };
    Damage maxDamage = -1;

    for (const auto& action : job.actions) {
        std::cerr << "Action loop - " << action.name() << std::endl;
        auto time = action.startTime(rot, gcdDelay);
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

// Always pick the action that will maximize <total damage> / <total duration>
Rotation greedyOptimalRotation(const Job& job, Time duration, Time gcdDelay)
{
    Rotation result;

    while (auto nextEntry = greedyChooseNextRotationEntry(result, job, duration, gcdDelay)) {
        result.entries.push_back(std::move(*nextEntry));
    }

    return result;
}

// Check every possible combination
Rotation exhaustiveOptimalRotation(const Job& job, Time duration, Time gcdDelay)
{
    Rotation workingRot;
    Damage maxDamage = -1;
    Rotation bestRot;

    std::vector<decltype(job.actions)::const_iterator> iters;
    std::vector<JobState> states;

    if (job.actions.empty())
        return bestRot;

    iters.push_back(job.actions.begin());
    states.emplace_back();
    while (!iters.empty()) {
        // check if can fit in move
        // if so calculate damage for current thing
        //    if best so far, note it
        //    add new iter
        // else increment current iter and pop until successful or empty

        /*
        std::cerr << "Loop: rotation = ";
        for (auto& [a, t] : workingRot.entries)
            std::cerr << a.name() << ", ";
        std::cerr << getName(*iters.back()) << std::endl;
        */

        auto nextTime = iters.back()->startTime(workingRot, gcdDelay);
        //std::cerr << "Next time = " << nextTime << std::endl;
        if (nextTime < duration) {
            auto [damage, state] = calculatePotentialDamageIncremental(*iters.back(), nextTime, duration, gcdDelay, states.back());
            //std::cerr << "Damage = " << damage << std::endl;
            /*
            if (duration - nextTime < 1.5)
                std::cerr << "DMG=" << damage << "\n";
                */
            /*
            if (damage > 1400 && duration - nextTime < gcdDelay) {
                //calculatePotentialDamage(workingRot, *iters.back(), nextTime, duration, gcdDelay, true);
                std::cerr << "New good DMG=" << damage;
                std::cerr << "; NextTime=" << nextTime << "; Loop: rotation = ";
                for (auto& [a, t] : workingRot.entries)
                    std::cerr << a.name() << ", ";
                std::cerr << getName(*iters.back()) << std::endl;
            }
            */

            workingRot.entries.push_back(RotationEntry { *iters.back(), nextTime });
            iters.push_back(job.actions.begin());
            states.push_back(state);

            if (damage > maxDamage) {
                maxDamage = damage;
                bestRot = workingRot;
            }
        } else {
            //std::cerr << "Popping" << std::endl;
            ++iters.back();
            while (iters.back() == job.actions.end()) {
                iters.pop_back();
                states.pop_back();
                if (iters.empty())
                    break;
                if (!workingRot.entries.empty())
                    workingRot.entries.pop_back();
                ++iters.back();
            }
        }
    }

    return bestRot;
}

// Does a faster search by pruning low-performing candidates after each generation.
// The criterion to maximize is damage/time with overall sequence time as a secondary criterion.
Rotation pruningOptimalRotation(const Job& job, Time duration, Time gcdDelay, int maxCandidates)
{
    using RotStatePair = std::pair<Rotation, JobState>;
    struct RotStatePairGreater {
        bool operator()(const RotStatePair& lhs, const RotStatePair& rhs) const
        {
            auto lhsDmg = lhs.first.entries.size() == 0 ? 0 : lhs.second.damage() / lhs.first.entries.back().time;
            auto rhsDmg = rhs.first.entries.size() == 0 ? 0 : rhs.second.damage() / rhs.first.entries.back().time;
            if (lhsDmg == rhsDmg)
                return lhs.first.entries.size() > rhs.first.entries.size();
            else
                return lhsDmg > rhsDmg;
        }
    };

    // Two priority queues for when we move between generations.
    std::priority_queue<RotStatePair, std::vector<RotStatePair>, RotStatePairGreater> pq1, pq2;
    Rotation bestRot;
    Damage maxDamage = -1;

    pq1.emplace();
    int generationNumber = 0;
    while (!pq1.empty()) {
        generationNumber++;
        std::cerr << "Generation " << generationNumber << " - pq1 size = " << pq1.size() << std::endl;
        while (!pq1.empty()) {
            // Check damage against best
            auto& [rot, state] = pq1.top();
            if (state.damage() > maxDamage) {
                // std::cerr << "New best: damage = " << state.damage() << std::endl;
                bestRot = rot;
                maxDamage = state.damage();
            }

            for (auto& action : job.actions) {
                // Put all time-viable candidates in pq2 ensuring it doesn't grow too large
                auto time = action.startTime(rot, gcdDelay);
                if (time - state.time() < gcdDelay * 1.1 && time < duration) {
                    auto newState = state;
                    newState.advanceTo(time);
                    newState.processAction(action);
                    if (newState.damage() >= state.damage()) {
                        auto newRot = rot;
                        newRot.entries.push_back({ action, time });
                        pq2.emplace(std::move(newRot), std::move(newState));
                        if (pq2.size() > maxCandidates)
                            pq2.pop();
                    }
                }
            }

            pq1.pop();
        }

        swap(pq1, pq2);
    }

    return bestRot;
}

Rotation calculateOptimalRotation(const Job& job, Time duration, Time gcdDelay, int maxCandidates)
{
    return pruningOptimalRotation(job, duration, gcdDelay, maxCandidates);
    //return exhaustiveOptimalRotation(job, duration, gcdDelay);
    //return greedyOptimalRotation(job, duration, gcdDelay);
}

void printResult(const Rotation& rotation, Time totalDamage)
{
    int counter = 0;
    std::cout << std::setprecision(2) << std::fixed;
    for (auto&& [action, time] : rotation.entries) {
        std::cout << counter++ << "\t" << time << "\t" << action.name() << std::endl;
    }
    std::cout << "\nTotal Damage: " << totalDamage << "\n";
}

int main(int argc, char** argv)
{
    std::string usage = "Usage:\n  "s + argv[0] + " <job> <duration in seconds> <GCD delay> [<num candidates = 500>]";
    if (argc != 4 && argc != 5) {
        std::cout << usage << std::endl;
        return 1;
    }

    std::string jobName = argv[1];
    Time duration = std::atof(argv[2]);
    Time gcdDelay = std::atof(argv[3]);
    int maxCandidates = 5000;
    if (argc == 5)
        maxCandidates = std::atoi(argv[4]);

    if (auto maybeJob = getJob(jobName)) {
        if (duration > 0 && gcdDelay > 0) {
            auto result = calculateOptimalRotation(*maybeJob, duration, gcdDelay, maxCandidates);
            auto totalDamage = calculatePotentialDamage(result, ACTID_Noop, duration, duration, gcdDelay, false);
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
