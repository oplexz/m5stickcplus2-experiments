#ifndef BLE_KEYBOARD_H
#define BLE_KEYBOARD_H

#include <Arduino.h>
#include <M5GFX.h>
#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>

class BleKeyboard
{
public:
    // passkey = 0 → Just Works (no PIN); passkey > 0 → display passkey pairing
    BleKeyboard(M5GFX& screen, const char* deviceName, uint32_t passkey = 0);

    void start();
    void stop();
    void tick();

    // Types a plain ASCII string as BLE keyboard keypresses
    void typeString(const char* str);

    void runMacro();

    bool isConnected() const;

private:
    static uint8_t charToKeycode(char c, uint8_t& modifierOut);

    void initBle();
    void sendKey(uint8_t modifier, uint8_t keycode);
    void drawScreen(uint32_t nowMs);
    template <typename T> void drawContent(T& display, uint32_t nowMs);

    M5GFX& screen_;
    M5Canvas canvas_;
    bool canvasReady_;

    const char* name_;
    uint32_t passkey_;

    bool active_;
    bool bleReady_;
    bool bleConnected_;
    bool bleAuthenticated_;
    uint32_t sentFlashMs_;
    uint32_t lastDrawMs_;

    NimBLEServer* server_;
    NimBLEHIDDevice* hid_;
    NimBLECharacteristic* inputKeyboard_;
    NimBLEServerCallbacks* serverCallbacks_;
};

#endif
