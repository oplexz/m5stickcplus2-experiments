#ifndef SCENARIO_RUNNER_H
#define SCENARIO_RUNNER_H

#include <Arduino.h>
#include <Scenario.h>

class BleHidKeyboard; // forward-declare — full header included in .cpp only

// Non-blocking tick-driven executor for a Scenario.
//
// All random wait durations are rolled at once at the start of each loop pass
// so the view can display the correct chosen value for every step, not just the
// current one.  Key/KeyDown/KeyUp steps are assigned kMinStepMs.
class ScenarioRunner
{
public:
    // Maximum number of actions per scenario supported.
    static constexpr int kMaxActions = 64;

    explicit ScenarioRunner(BleHidKeyboard& transport);

    void start(const Scenario* scenario, uint32_t now);
    void stop();
    void tick(uint32_t now);

    bool            isRunning()          const;
    int             currentActionIndex() const;
    const Scenario* activeScenario()     const;
    int             loopCount()          const { return loopCount_; }

    // Pre-rolled durations for every action in the active scenario.
    // Index matches the scenario's actions array.
    const uint32_t* chosenWaits() const { return chosenWaits_; }

private:
    void rollWaits();
    void enterAction(int index, uint32_t now);

    static constexpr uint32_t kMinStepMs = 200;

    BleHidKeyboard& transport_;
    const Scenario* scenario_;
    int             actionIndex_;
    uint32_t        stepDeadline_;
    bool            running_;
    int             loopCount_;

    uint32_t chosenWaits_[kMaxActions];
};

#endif
