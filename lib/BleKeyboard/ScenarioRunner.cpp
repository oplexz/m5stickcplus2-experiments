#include "ScenarioRunner.h"
#include "BleHidKeyboard.h"

ScenarioRunner::ScenarioRunner(BleHidKeyboard& transport)
: transport_(transport)
, scenario_(nullptr)
, actionIndex_(0)
, stepDeadline_(0)
, running_(false)
, loopCount_(0)
{
    memset(chosenWaits_, 0, sizeof(chosenWaits_));
}

void ScenarioRunner::start(const Scenario* scenario, uint32_t now)
{
    if (!scenario || scenario->count == 0) return;
    scenario_    = scenario;
    actionIndex_ = 0;
    running_     = true;
    loopCount_   = 0;
    rollWaits();
    enterAction(0, now);
}

void ScenarioRunner::stop()
{
    transport_.keyUp(); // release any held keys
    running_     = false;
    scenario_    = nullptr;
    actionIndex_ = 0;
    memset(chosenWaits_, 0, sizeof(chosenWaits_));
}

void ScenarioRunner::tick(uint32_t now)
{
    if (!running_) return;
    if (now < stepDeadline_) return;

    actionIndex_++;
    if (actionIndex_ >= static_cast<int>(scenario_->count))
    {
        if (scenario_->mode == RunMode::Loop)
        {
            loopCount_++;
            actionIndex_ = 0;
            rollWaits(); // fresh random values for the next pass
            enterAction(0, now);
        }
        else
        {
            stop();
        }
        return;
    }
    enterAction(actionIndex_, now);
}

bool            ScenarioRunner::isRunning()          const { return running_; }
int             ScenarioRunner::currentActionIndex() const { return actionIndex_; }
const Scenario* ScenarioRunner::activeScenario()     const { return scenario_; }

void ScenarioRunner::rollWaits()
{
    int count = static_cast<int>(scenario_->count);
    if (count > kMaxActions) count = kMaxActions;

    for (int i = 0; i < count; ++i)
    {
        const Action& a = scenario_->actions[i];
        if (a.kind == ActionKind::Wait)
        {
            // Spike: roll a 1-in-N chance of using the AFK range instead.
            bool useSpike = (a.spikeOneIn > 0) && (random(static_cast<long>(a.spikeOneIn)) == 0);

            uint32_t lo = useSpike ? a.spikeMinMs : a.waitMinMs;
            uint32_t hi = useSpike ? a.spikeMaxMs : a.waitMaxMs;
            chosenWaits_[i] = (hi > lo)
                ? static_cast<uint32_t>(random(static_cast<long>(lo),
                                               static_cast<long>(hi) + 1))
                : lo;
        }
        else
        {
            chosenWaits_[i] = kMinStepMs;
        }
    }
}

void ScenarioRunner::enterAction(int index, uint32_t now)
{
    const Action& a = scenario_->actions[index];

    if (a.kind == ActionKind::Key)
    {
        transport_.sendKey(a.modifier, a.keycode);
    }
    else if (a.kind == ActionKind::KeyDown)
    {
        transport_.keyDown(a.modifier, a.keycode);
    }
    else if (a.kind == ActionKind::KeyUp)
    {
        transport_.keyUp();
    }
    // Wait: no transport call, just wait out the deadline

    stepDeadline_ = now + chosenWaits_[index];
}
