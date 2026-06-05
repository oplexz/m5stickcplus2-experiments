#ifndef BLE_KEYBOARD_VIEW_H
#define BLE_KEYBOARD_VIEW_H

#include <Arduino.h>
#include <M5GFX.h>
#include <Theme.h>
#include <Ui.h>
#include <cstdio>

namespace BleKeyboardView
{
    constexpr uint32_t kSentFlashDurationMs = 600;

    struct State
    {
        bool        connected;
        bool        authenticated;
        const char* name;
        uint32_t    passkey;
        uint32_t    sentFlashMs;
    };

    // Template so the same code renders to M5GFX (direct) or M5Canvas (buffered).
    template <typename GFX>
    void drawContent(GFX& d, const State& s, uint32_t nowMs, int16_t w, int16_t h)
    {
        const int16_t midY = h / 2 - 8;

        d.fillScreen(Theme::kBg);

        // ── top bar ───────────────────────────────────────────────────────────
        d.setTextSize(Theme::kSzSmall);
        d.setTextColor(Theme::kText, Theme::kBg);
        d.setCursor(3, 4);
        d.print(s.name);

        d.setCursor(w - 30, 4);
        if      (s.authenticated) { d.setTextColor(Theme::kOk,     Theme::kBg); d.print("BLE+"); }
        else if (s.connected)     { d.setTextColor(Theme::kTitle,   Theme::kBg); d.print("BLE~"); }
        else                      { d.setTextColor(Theme::kHint,    Theme::kBg); d.print("BLE-"); }
        d.setTextColor(Theme::kText, Theme::kBg);

        d.drawFastHLine(0, 14, w, Theme::kSep);

        // ── middle content ────────────────────────────────────────────────────
        bool showSent = (s.sentFlashMs != 0 && nowMs - s.sentFlashMs < kSentFlashDurationMs);

        if (showSent)
        {
            d.setTextSize(Theme::kSzNormal);
            d.setTextColor(Theme::kTitle, Theme::kBg);
            d.setCursor(w / 2 - 24, midY);
            d.print("Sent!");
            d.setTextSize(Theme::kSzSmall);
            d.setTextColor(Theme::kText, Theme::kBg);
        }
        else if (s.authenticated)
        {
            d.setCursor(3, midY);
            d.print("Press btn");
            d.setTextColor(Theme::kActive, Theme::kBg);
            d.setCursor(3, midY + 12);
            d.print("  -> macro");
            d.setTextColor(Theme::kText, Theme::kBg);
        }
        else if (s.connected)
        {
            d.setTextColor(Theme::kTitle, Theme::kBg);
            d.setCursor(3, midY - 4);
            d.print("Authenticating...");
            if (s.passkey > 0)
            {
                char pinStr[12];
                snprintf(pinStr, sizeof(pinStr), "%lu", static_cast<unsigned long>(s.passkey));
                int16_t pinX = (w - static_cast<int16_t>(strlen(pinStr)) * 12) / 2;
                d.setTextSize(Theme::kSzNormal);
                d.setTextColor(Theme::kActive, Theme::kBg);
                d.setCursor(pinX, midY + 10);
                d.print(pinStr);
                d.setTextSize(Theme::kSzSmall);
            }
            d.setTextColor(Theme::kText, Theme::kBg);
        }
        else
        {
            if (s.passkey > 0)
            {
                char pinStr[12];
                snprintf(pinStr, sizeof(pinStr), "%lu", static_cast<unsigned long>(s.passkey));
                int16_t pinX = (w - static_cast<int16_t>(strlen(pinStr)) * 12) / 2;
                d.setTextColor(Theme::kHint, Theme::kBg);
                d.setCursor(3, midY - 8);
                d.print("Advertising... PIN:");
                d.setTextSize(Theme::kSzNormal);
                d.setTextColor(Theme::kActive, Theme::kBg);
                d.setCursor(pinX, midY + 6);
                d.print(pinStr);
                d.setTextSize(Theme::kSzSmall);
                d.setTextColor(Theme::kText, Theme::kBg);
            }
            else
            {
                d.setCursor(3, midY);
                d.print("Advertising...");
            }
        }

        // ── bottom hint ───────────────────────────────────────────────────────
        d.drawFastHLine(0, h - 14, w, Theme::kSep);
        d.setTextColor(Theme::kHint, Theme::kBg);
        d.setCursor(3, h - 10);
        d.print("Short=run  Hold=back");
        d.setTextColor(Theme::kText, Theme::kBg);
    }
}

#endif
