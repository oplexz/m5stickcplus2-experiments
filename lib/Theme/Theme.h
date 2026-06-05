#ifndef THEME_H
#define THEME_H

#include <Arduino.h>
#include <M5GFX.h>

// Central design token file. Everything that touches colour or position
// should reference these constants rather than using magic numbers.
namespace Theme
{
    // ── Colors ────────────────────────────────────────────────────────────────
    constexpr uint16_t kBg        = TFT_BLACK;
    constexpr uint16_t kText      = TFT_WHITE;
    constexpr uint16_t kTitle     = TFT_YELLOW;
    constexpr uint16_t kDim       = TFT_DARKGREY;
    constexpr uint16_t kHint      = 0x8410;   // muted grey for footer hints
    constexpr uint16_t kSep       = 0x4208;   // thin separator lines
    constexpr uint16_t kOk        = TFT_GREEN;
    constexpr uint16_t kActive    = TFT_CYAN;
    constexpr uint16_t kListSelBg = TFT_DARKGREY;
    constexpr uint16_t kListSelFg = TFT_BLACK;

    // ── Text sizes ────────────────────────────────────────────────────────────
    constexpr int kSzSmall  = 1;
    constexpr int kSzNormal = 2;
    constexpr int kSzLarge  = 3;

    // ── Layout metrics ────────────────────────────────────────────────────────
    constexpr int kMarginX       = 8;
    constexpr int kTitleY        = 4;
    constexpr int kHintY         = 120;
    constexpr int kLineHeight    = 20;
    constexpr int kListTop       = 8;
    constexpr int kListLeft      = 8;
    constexpr int kScrollPadding = 4;
}

#endif
