#ifndef HID_KEYCODES_H
#define HID_KEYCODES_H

#include <Arduino.h>

// USB HID keyboard scan codes and modifier bitmask definitions.
// Extracted from the original BleKeyboard anonymous namespace so they can be
// shared between BleHidKeyboard (transport) and BleKeyboard (macro).
namespace HidKeycodes
{
    // ── Modifier bits ─────────────────────────────────────────────────────────
    constexpr uint8_t kModCtrl  = 0x01;
    constexpr uint8_t kModShift = 0x02;
    constexpr uint8_t kModAlt   = 0x04;
    constexpr uint8_t kModGui   = 0x08; // Win / Cmd

    // ── Special keys ──────────────────────────────────────────────────────────
    constexpr uint8_t kEnter  = 0x28;
    constexpr uint8_t kEsc    = 0x29;
    constexpr uint8_t kTab    = 0x2B;
    constexpr uint8_t kSpace  = 0x2C;
    constexpr uint8_t kBacksp = 0x2A;

    // ── Arrow keys ────────────────────────────────────────────────────────────
    constexpr uint8_t kRight  = 0x4F;
    constexpr uint8_t kLeft   = 0x50;
    constexpr uint8_t kDown   = 0x51;
    constexpr uint8_t kUp     = 0x52;

    // ── Letter keys (a=0x04 … z=0x1D) ────────────────────────────────────────
    constexpr uint8_t kA = 0x04, kB = 0x05, kC = 0x06, kD = 0x07, kE = 0x08;
    constexpr uint8_t kF = 0x09, kG = 0x0A, kH = 0x0B, kI = 0x0C, kJ = 0x0D;
    constexpr uint8_t kK = 0x0E, kL = 0x0F, kM = 0x10, kN = 0x11, kO = 0x12;
    constexpr uint8_t kP = 0x13, kQ = 0x14, kR = 0x15, kS = 0x16, kT = 0x17;
    constexpr uint8_t kU = 0x18, kV = 0x19, kW = 0x1A, kX = 0x1B, kY = 0x1C;
    constexpr uint8_t kZ = 0x1D;

    // ── Function keys ─────────────────────────────────────────────────────────
    constexpr uint8_t kF1  = 0x3A, kF2  = 0x3B, kF3  = 0x3C, kF4  = 0x3D;
    constexpr uint8_t kF5  = 0x3E, kF6  = 0x3F, kF7  = 0x40, kF8  = 0x41;
    constexpr uint8_t kF9  = 0x42, kF10 = 0x43, kF11 = 0x44, kF12 = 0x45;

    // Map a printable ASCII character to a HID scan code.
    // Sets modifier to kModShift if the key requires shift, 0 otherwise.
    // Returns 0 if the character is not mappable.
    inline uint8_t charToKeycode(char c, uint8_t& modifier)
    {
        modifier = 0;
        if (c >= 'a' && c <= 'z') return static_cast<uint8_t>(0x04 + (c - 'a'));
        if (c >= 'A' && c <= 'Z') { modifier = kModShift; return static_cast<uint8_t>(0x04 + (c - 'A')); }
        if (c == ' ')              return 0x2C;
        if (c == '\n' || c == '\r') return 0x28;
        if (c == '\t')             return 0x2B;
        return 0;
    }
}

#endif
