#ifndef BLE_KEYBOARD_H
#define BLE_KEYBOARD_H

#include <Arduino.h>
#include <M5GFX.h>
#include "BleHidKeyboard.h"

// Thin orchestrator: owns the BLE transport (BleHidKeyboard), the display
// canvas, the macro sequence, and display-state timing. All NimBLE and HID
// logic lives in BleHidKeyboard; all drawing logic lives in BleKeyboardView.
class BleKeyboard
{
public:
    // passkey = 0 → Just Works (no PIN); passkey > 0 → display passkey pairing
    BleKeyboard(M5GFX& screen, const char* deviceName, uint32_t passkey = 0);

    void start();
    void stop();
    void tick();

    void typeString(const char* str);
    void runMacro();

    bool isConnected() const;

private:
    void drawScreen(uint32_t nowMs);

    M5GFX&         screen_;
    M5Canvas       canvas_;
    bool           canvasReady_;
    BleHidKeyboard transport_;
    const char*    name_;
    uint32_t       passkey_;

    bool     active_;
    uint32_t sentFlashMs_;
    uint32_t lastDrawMs_;

    static constexpr uint32_t kDrawIntervalMs = 100;
};

#endif
