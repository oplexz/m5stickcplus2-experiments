#include "Scenarios.h"

// ── HID keycode constants (local to this file) ────────────────────────────────
// Mirrors HidKeycodes.h but lives here to avoid a cross-lib include dependency.
namespace
{
    constexpr uint8_t kEnter = 0x28;
    constexpr uint8_t kEsc   = 0x29;
    constexpr uint8_t kW     = 0x1A;
    constexpr uint8_t kX     = 0x1B;
    constexpr uint8_t kY     = 0x1C;
    constexpr uint8_t kDown  = 0x51;
    constexpr uint8_t kRight = 0x4F;
    constexpr uint8_t kSpace = 0x2C;
}

// ── Scenario 1: Main Macro (run once) ────────────────────────────────────────
// Migrated verbatim from the original blocking runMacro().
// Each key press is paired with a randomised wait that matches the original
// human-like timing ranges.
static const Action kMainMacroActions[] = {
    keyAction(kEnter,           "Enter"),
    waitAction(300,  400),
    keyAction(kEnter,           "Enter"),
    waitAction(900,  1000),
    keyAction(kY,               "Y"),
    waitAction(205,  305),
    keyAction(kDown,            "Down"),
    waitAction(50,   150),
    keyAction(kEnter,           "Enter"),
    waitAction(200,  300),
    keyAction(kEnter,           "Enter"),
    waitAction(200,  300),
    keyAction(kEsc,             "Esc"),
    waitAction(75,   150),
    keyAction(kEsc,             "Esc"),
    waitAction(75,   150),
    keyAction(kEsc,             "Esc"),
    waitAction(75,   150),
    keyAction(kEsc,             "Esc"),
    waitAction(75,   150),
    keyAction(kEsc,             "Esc"),
    waitAction(700,  800),
};

// ── Scenario 2: Slide Forward (loop) ─────────────────────────────────────────
// Sends Right every 10 seconds. Useful for auto-advancing presentations or
// keeping focus alive. Loops until cancelled.
static const Action kSlideFwdActions[] = {
    keyAction(kRight,           "Right"),
    waitAction(10000, 10000,    "10s idle"),
};

// ── Scenario 3: Hold Space (loop) ────────────────────────────────────────────
// Holds Space for 3 seconds, releases, waits 2 seconds, then repeats.
// Demonstrates KeyDown / KeyUp step types.
static const Action kHoldSpaceActions[] = {
    keyDownAction(kSpace,           "Space down"),
    waitAction(3000, 3000,          "3s hold"),
    keyUpAction(                    "release"),
    waitAction(2000, 2000,          "2s idle"),
};

// ── Scenario 4: Space Loop ────────────────────────────────────────────────────
// Sends Space on a short interval — handy for keeping a remote session alive
// or holding a game button. Loops until cancelled.
static const Action kSpaceLoopActions[] = {
    keyAction(kSpace,           "Space"),
    waitAction(5000, 5000,      "5s idle"),
};

// ── Scenario 5: Race Loop ─────────────────────────────────────────────────────
// Plays a full race cycle and loops back to the start for the next race.
static const Action kRaceLoopActions[] = {
    keyAction(kEnter,               "Start race"),
    waitAction(2500,  3500,         "Wait for countdown"),
    keyDownAction(kW,               "Go forward"),
    waitAction(15000, 19000,        "Race"),
    keyUpAction(                    "Stop forward"),
    waitAction(10000, 18000,        "Wait for leaderboard"),
    keyAction(kX,                   "Restart"),
    waitAction(300,   1000,         "Wait for prompt"),
    keyAction(kEnter,               "Confirm restart"),
    // Normally 5-10s; ~1% chance per loop (≈once/hour) of a 3-10 min AFK spike.
    waitActionSpiked(15000, 30000,  100, 180000, 600000, "Wait before start"),
};

// ── Catalogue ─────────────────────────────────────────────────────────────────

const Scenario kScenarios[] = {
    { "Main Macro",   RunMode::Once, kMainMacroActions,  sizeof(kMainMacroActions)  / sizeof(kMainMacroActions[0])  },
    { "Race Loop",    RunMode::Loop, kRaceLoopActions,   sizeof(kRaceLoopActions)   / sizeof(kRaceLoopActions[0])   },
    { "Slide Fwd",    RunMode::Loop, kSlideFwdActions,   sizeof(kSlideFwdActions)   / sizeof(kSlideFwdActions[0])   },
    { "Hold Space",   RunMode::Loop, kHoldSpaceActions,  sizeof(kHoldSpaceActions)  / sizeof(kHoldSpaceActions[0])  },
    { "Space Loop",   RunMode::Loop, kSpaceLoopActions,  sizeof(kSpaceLoopActions)  / sizeof(kSpaceLoopActions[0])  },
};

const size_t kScenarioCount = sizeof(kScenarios) / sizeof(kScenarios[0]);
