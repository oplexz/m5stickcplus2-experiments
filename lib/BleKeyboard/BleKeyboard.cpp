#include "BleKeyboard.h"
#include "BleKeyboardView.h"
#include <Scenarios.h>

BleKeyboard::BleKeyboard(M5GFX& screen, const char* deviceName, uint32_t passkey)
: screen_(screen)
, canvas_(&screen)
, canvasReady_(false)
, transport_(deviceName, passkey)
, runner_(transport_)
, name_(deviceName)
, passkey_(passkey)
, active_(false)
, selectedScenario_(0)
, doneFlashMs_(0)
, lastDrawMs_(0)
, dirty_(false)
{}

void BleKeyboard::start()
{
    active_           = true;
    doneFlashMs_      = 0;
    lastDrawMs_       = 0;
    dirty_            = true;
    selectedScenario_ = 0;

    if (!canvasReady_)
    {
        canvas_.setColorDepth(16);
        canvasReady_ = (canvas_.createSprite(screen_.width(), screen_.height()) != nullptr);
    }

    if (!transport_.isReady())
    {
        transport_.init();
    }
    else if (!transport_.isAdvertising())
    {
        transport_.startAdvertising();
    }
}

void BleKeyboard::stop()
{
    active_ = false;
    runner_.stop();
    transport_.stop();
}

void BleKeyboard::tick()
{
    if (!active_) return;

    uint32_t now = millis();

    bool wasRunning = runner_.isRunning();
    runner_.tick(now);
    bool isNowRunning = runner_.isRunning();

    // Detect Once-scenario completion to flash "Done!".
    if (wasRunning && !isNowRunning)
    {
        doneFlashMs_ = now;
        dirty_       = true;
    }

    // Redraw on interval or when marked dirty (selection change, step advance).
    if (dirty_ || (now - lastDrawMs_ >= kDrawIntervalMs))
    {
        lastDrawMs_ = now;
        dirty_      = false;
        drawScreen(now);
    }
}

void BleKeyboard::selectPrev()
{
    if (runner_.isRunning()) return;
    selectedScenario_ = (selectedScenario_ == 0)
                        ? static_cast<int>(kScenarioCount) - 1
                        : selectedScenario_ - 1;
    dirty_ = true;
}

void BleKeyboard::selectNext()
{
    if (runner_.isRunning()) return;
    selectedScenario_ = (selectedScenario_ + 1) % static_cast<int>(kScenarioCount);
    dirty_ = true;
}

void BleKeyboard::activate(uint32_t now)
{
    if (!transport_.isAuthenticated()) return;
    if (selectedScenario_ < 0 || selectedScenario_ >= static_cast<int>(kScenarioCount)) return;
    doneFlashMs_ = 0;
    runner_.start(&kScenarios[selectedScenario_], now);
    dirty_ = true;
}

void BleKeyboard::cancel()
{
    runner_.stop();
    dirty_ = true;
}

bool BleKeyboard::isRunning()   const { return runner_.isRunning(); }
bool BleKeyboard::isConnected() const { return transport_.isAuthenticated(); }

void BleKeyboard::drawScreen(uint32_t nowMs)
{
    BleKeyboardView::State state{
        transport_.isConnected(),
        transport_.isAuthenticated(),
        name_,
        passkey_,
        kScenarios,
        kScenarioCount,
        selectedScenario_,
        runner_.isRunning(),
        runner_.activeScenario(),
        runner_.currentActionIndex(),
        runner_.chosenWaits(),
        runner_.loopCount(),
        doneFlashMs_,
    };

    if (canvasReady_)
    {
        BleKeyboardView::drawContent(canvas_, state, nowMs, screen_.width(), screen_.height());
        canvas_.pushSprite(0, 0);
    }
    else
    {
        BleKeyboardView::drawContent(screen_, state, nowMs, screen_.width(), screen_.height());
    }
}
