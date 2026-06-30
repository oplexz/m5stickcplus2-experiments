#ifndef SCENARIO_H
#define SCENARIO_H

#include <Arduino.h>
#include "Action.h"

enum class RunMode
{
    Once,   // play actions once, then stop
    Loop,   // wrap back to action 0 after the last action
};

struct Scenario
{
    const char*   name;
    RunMode       mode;
    const Action* actions;
    size_t        count;
};

#endif
