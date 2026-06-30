#ifndef BLE_KEYBOARD_H
#define BLE_KEYBOARD_H

#include <Arduino.h>
#include <M5GFX.h>
#include "BleHidKeyboard.h"
#include "ScenarioRunner.h"

// Thin orchestrator: owns the BLE transport (BleHidKeyboard), the display
// canvas, the scenario runner, and scenario selection state.
// All NimBLE / HID logic lives in BleHidKeyboard.
// All drawing logic lives in BleKeyboardView.
class BleKeyboard
{
public:
    BleKeyboard(M5GFX& screen, const char* deviceName, uint32_t passkey = 0);

    void start();
    void stop();
    void tick();

    // Scenario selection (idle-only; ignored while running).
    void selectPrev();
    void selectNext();

    // Start executing the selected scenario (no-op if not authenticated).
    void activate(uint32_t now);

    // Interrupt a running scenario immediately.
    void cancel();

    bool isRunning()   const;
    bool isConnected() const;

private:
    void drawScreen(uint32_t nowMs);

    M5GFX&          screen_;
    M5Canvas        canvas_;
    bool            canvasReady_;
    BleHidKeyboard  transport_;
    ScenarioRunner  runner_;
    const char*     name_;
    uint32_t        passkey_;

    bool     active_;
    int      selectedScenario_;
    uint32_t doneFlashMs_;
    uint32_t lastDrawMs_;
    bool     dirty_;

    static constexpr uint32_t kDrawIntervalMs = 100;
};

#endif
