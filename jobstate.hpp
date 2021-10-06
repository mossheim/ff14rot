#pragma once

#include "actions.hpp"

struct JobState
{
    Damage advanceTo(Time time) { return 0; }
    Damage processAction(const Action& action) { return 0; }
};
