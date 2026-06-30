#ifndef ACTION_H
#define ACTION_H

#include <Arduino.h>

enum class ActionKind
{
    Key,     // send one key press+release, then dwell kMinStepMs
    KeyDown, // send key-down report only (key stays held until KeyUp)
    KeyUp,   // send all-zeros release report (modifier/keycode ignored)
    Wait,    // non-blocking delay — supports optional random spike
};

// A single step in a Scenario.
//
// For Key / KeyDown / KeyUp: modifier + keycode are HID codes; wait fields are 0.
// For Wait: modifier/keycode are 0; waitMinMs/waitMaxMs define the normal range.
//   Spike: if spikeOneIn > 0, there is a 1-in-spikeOneIn chance per roll that the
//   delay is instead drawn from [spikeMinMs, spikeMaxMs] (e.g. a simulated AFK).
struct Action
{
    ActionKind  kind;
    uint8_t     modifier;
    uint8_t     keycode;
    uint32_t    waitMinMs;
    uint32_t    waitMaxMs;
    const char* label;        // short human-readable name shown in the action list

    // Optional spike (only used when kind == Wait and spikeOneIn > 0).
    uint16_t spikeOneIn;      // 0 = disabled; N = 1-in-N chance of using spike range
    uint32_t spikeMinMs;
    uint32_t spikeMaxMs;
};

// ── Factory helpers ───────────────────────────────────────────────────────────

inline Action keyAction(uint8_t modifier, uint8_t keycode, const char* label)
{
    return { ActionKind::Key, modifier, keycode, 0, 0, label, 0, 0, 0 };
}

// Convenience: no modifier
inline Action keyAction(uint8_t keycode, const char* label)
{
    return keyAction(0x00, keycode, label);
}

inline Action keyDownAction(uint8_t modifier, uint8_t keycode, const char* label)
{
    return { ActionKind::KeyDown, modifier, keycode, 0, 0, label, 0, 0, 0 };
}

inline Action keyDownAction(uint8_t keycode, const char* label)
{
    return keyDownAction(0x00, keycode, label);
}

inline Action keyUpAction(const char* label = "release")
{
    return { ActionKind::KeyUp, 0, 0, 0, 0, label, 0, 0, 0 };
}

inline Action waitAction(uint32_t minMs, uint32_t maxMs, const char* label = "wait")
{
    return { ActionKind::Wait, 0, 0, minMs, maxMs, label, 0, 0, 0 };
}

// Wait with an occasional spike (e.g. a simulated AFK pause).
// spikeOneIn = N  →  1-in-N chance per loop that this step uses [spikeMinMs, spikeMaxMs]
// instead of [minMs, maxMs].  When it fires, the display shows the spike duration.
inline Action waitActionSpiked(uint32_t minMs, uint32_t maxMs,
                               uint16_t spikeOneIn,
                               uint32_t spikeMinMs, uint32_t spikeMaxMs,
                               const char* label = "wait")
{
    return { ActionKind::Wait, 0, 0, minMs, maxMs, label, spikeOneIn, spikeMinMs, spikeMaxMs };
}

#endif
