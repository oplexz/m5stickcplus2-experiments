#ifndef BLE_HID_KEYBOARD_H
#define BLE_HID_KEYBOARD_H

#include <Arduino.h>
#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>
#include "HidKeycodes.h"

// NimBLE HID keyboard transport layer. Owns the BLE stack, HID descriptor,
// advertising, security, and raw key sending. No display logic lives here.
class BleHidKeyboard
{
public:
    BleHidKeyboard(const char* name, uint32_t passkey);

    // One-time BLE + HID initialisation and advertising start.
    void init();

    // Advertising control.
    void startAdvertising();
    void stop();           // stop advertising and release all keys

    bool isReady()         const;
    bool isAdvertising()   const;
    bool isConnected()     const;
    bool isAuthenticated() const;

    // Send a single key press+release report (atomic).
    void sendKey(uint8_t modifier, uint8_t keycode);

    // Send only the key-down report — key stays held until keyUp() is called.
    void keyDown(uint8_t modifier, uint8_t keycode);

    // Send the all-zeros release report (releases all held keys).
    void keyUp();

    // Type a plain ASCII string one character at a time.
    void typeString(const char* str);

private:
    void initBle();

    const char*            name_;
    uint32_t               passkey_;
    bool                   bleReady_;
    bool                   connected_;
    bool                   authenticated_;

    NimBLEServer*          server_;
    NimBLEHIDDevice*       hid_;
    NimBLECharacteristic*  inputKeyboard_;
    NimBLEServerCallbacks* serverCallbacks_;

    static uint8_t kReportMap_[];
};

#endif
