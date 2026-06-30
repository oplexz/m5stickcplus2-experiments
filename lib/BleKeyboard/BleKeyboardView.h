#ifndef BLE_KEYBOARD_VIEW_H
#define BLE_KEYBOARD_VIEW_H

#include <Arduino.h>
#include <M5GFX.h>
#include <Theme.h>
#include <Ui.h>
#include <Scenario.h>
#include <cstdio>
#include <cstring>

namespace BleKeyboardView
{
    constexpr uint32_t kDoneFlashMs = 1200;

    struct State
    {
        // Top-bar connection status
        bool        connected;
        bool        authenticated;
        const char* name;
        uint32_t    passkey;

        // Scenario catalogue (shown when authenticated + idle)
        const Scenario* scenarios;
        size_t          scenarioCount;
        int             selectedScenario;

        // Live execution state
        bool            running;
        const Scenario* activeScenario;   // null when idle
        int             currentActionIdx;
        const uint32_t* chosenWaits;      // pre-rolled duration for every action, indexed by action pos
        int             loopCount;        // completed loop iterations (0 = first pass in progress)

        // Post-Once-completion flash
        uint32_t doneFlashMs; // millis() at completion, 0 = not active
    };

    // ── Internal layout constants ────────────────────────────────────────────
    namespace L
    {
        constexpr int kTopBarH   = 15;  // top separator y
        constexpr int kBotBarH   = 14;  // bottom bar height
        constexpr int kContentY  = 17;  // content area top
        constexpr int kActionLH  = 12;  // action-list line height (textSize 1)
    }

    // ── Helpers ──────────────────────────────────────────────────────────────

    // Format a duration into a short human-readable string:
    //   < 1 000 ms  →  "Xms"
    //   < 60 000 ms →  "Xs"
    //   >= 60 000 ms → "Xm Ys"
    inline void fmtDuration(char* buf, int len, uint32_t ms)
    {
        if (ms < 1000)
            snprintf(buf, len, "%lums", static_cast<unsigned long>(ms));
        else if (ms < 60000)
            snprintf(buf, len, "%lus", static_cast<unsigned long>(ms / 1000));
        else
        {
            unsigned long m = ms / 60000;
            unsigned long s = (ms % 60000) / 1000;
            snprintf(buf, len, "%lum%lus", m, s);
        }
    }

    // Compute a scroll top-index that keeps 'centerIdx' roughly centred within
    // the visible window. count is the total number of items.
    inline int centeredTopIndex(int centerIdx, int count, int rows)
    {
        int top = centerIdx - rows / 2;
        int maxTop = count - rows;
        if (top > maxTop) top = maxTop;
        if (top < 0)      top = 0;
        return top;
    }

    // ── Sub-renderers ────────────────────────────────────────────────────────

    template <typename GFX>
    void drawTopBar(GFX& d, const State& s, int16_t w)
    {
        d.setTextSize(Theme::kSzSmall);
        d.setTextColor(Theme::kText, Theme::kBg);
        d.setCursor(3, 4);
        d.print(s.name);

        d.setCursor(w - 30, 4);
        if      (s.authenticated) { d.setTextColor(Theme::kOk,   Theme::kBg); d.print("BLE+"); }
        else if (s.connected)     { d.setTextColor(Theme::kTitle, Theme::kBg); d.print("BLE~"); }
        else                      { d.setTextColor(Theme::kHint,  Theme::kBg); d.print("BLE-"); }
        d.setTextColor(Theme::kText, Theme::kBg);
        d.drawFastHLine(0, L::kTopBarH, w, Theme::kSep);
    }

    template <typename GFX>
    void drawBottomHint(GFX& d, const State& s, uint32_t nowMs, int16_t w, int16_t h)
    {
        d.drawFastHLine(0, h - L::kBotBarH, w, Theme::kSep);

        // "Done!" flash after a Once scenario completes.
        if (!s.running && s.doneFlashMs != 0 && nowMs - s.doneFlashMs < kDoneFlashMs)
        {
            d.setTextSize(Theme::kSzSmall);
            d.setTextColor(Theme::kOk, Theme::kBg);
            d.setCursor(3, h - 10);
            d.print("Done!");
            d.setTextColor(Theme::kText, Theme::kBg);
            return;
        }

        d.setTextSize(Theme::kSzSmall);
        d.setCursor(3, h - 10);
        if (s.running && s.activeScenario && s.activeScenario->mode == RunMode::Loop)
        {
            // Left: loop counter + estimated skill points
            int completed = s.loopCount;
            int sp        = completed * 10;
            char stat[24];
            snprintf(stat, sizeof(stat), "x%d ~%dsp", completed, sp);
            d.setTextColor(Theme::kActive, Theme::kBg);
            d.print(stat);

            // Right: stop hint
            d.setTextColor(Theme::kHint, Theme::kBg);
            int hw = 10 * 6; // "Sel=stop" ≈ 8 chars × 6px + margin
            d.setCursor(w - hw - Theme::kMarginX, h - 10);
            d.print("Sel=stop");
        }
        else if (s.running)
        {
            d.setTextColor(Theme::kHint, Theme::kBg);
            d.print("Sel=stop    Hold=back");
        }
        else
        {
            d.setTextColor(Theme::kHint, Theme::kBg);
            d.print("Sel=run  Hold=back");
        }
        d.setTextColor(Theme::kText, Theme::kBg);
    }

    // Render the advertising / pairing / pin UI (unauthenticated state).
    template <typename GFX>
    void drawConnectionContent(GFX& d, const State& s, int16_t w, int16_t h)
    {
        const int16_t midY = h / 2 - 8;

        if (s.connected)
        {
            d.setTextSize(Theme::kSzSmall);
            d.setTextColor(Theme::kTitle, Theme::kBg);
            d.setCursor(3, midY - 4);
            d.print("Authenticating...");
            if (s.passkey > 0)
            {
                char pin[12];
                snprintf(pin, sizeof(pin), "%lu", static_cast<unsigned long>(s.passkey));
                int16_t pinX = (w - static_cast<int16_t>(strlen(pin)) * 12) / 2;
                d.setTextSize(Theme::kSzNormal);
                d.setTextColor(Theme::kActive, Theme::kBg);
                d.setCursor(pinX, midY + 10);
                d.print(pin);
                d.setTextSize(Theme::kSzSmall);
            }
            d.setTextColor(Theme::kText, Theme::kBg);
        }
        else
        {
            if (s.passkey > 0)
            {
                char pin[12];
                snprintf(pin, sizeof(pin), "%lu", static_cast<unsigned long>(s.passkey));
                int16_t pinX = (w - static_cast<int16_t>(strlen(pin)) * 12) / 2;
                d.setTextSize(Theme::kSzSmall);
                d.setTextColor(Theme::kHint, Theme::kBg);
                d.setCursor(3, midY - 8);
                d.print("Advertising... PIN:");
                d.setTextSize(Theme::kSzNormal);
                d.setTextColor(Theme::kActive, Theme::kBg);
                d.setCursor(pinX, midY + 6);
                d.print(pin);
                d.setTextSize(Theme::kSzSmall);
                d.setTextColor(Theme::kText, Theme::kBg);
            }
            else
            {
                d.setTextSize(Theme::kSzSmall);
                d.setCursor(3, midY);
                d.print("Advertising...");
            }
        }
    }

    // Render the scenario selection list.
    template <typename GFX>
    void drawScenarioList(GFX& d, const State& s, int16_t w, int16_t h)
    {
        Ui::ListLayout layout;
        layout.top          = L::kContentY;
        layout.left         = Theme::kListLeft;
        layout.lineHeight   = Theme::kLineHeight;
        layout.textSize     = Theme::kSzNormal;
        layout.scrollPadding = 2;

        Ui::ListColors colors;

        int rows   = (h - L::kBotBarH - layout.top - layout.scrollPadding) / layout.lineHeight;
        int topIdx = centeredTopIndex(s.selectedScenario, static_cast<int>(s.scenarioCount), rows);

        Ui::listRows(d, layout, colors, topIdx, static_cast<int>(s.scenarioCount),
                     s.selectedScenario,
                     [&](GFX& g2, int i, int y, bool isSel)
                     {
                         g2.setCursor(layout.left, y);
                         g2.print(s.scenarios[i].name);

                         // Mode badge on the right (textSize 1)
                         const char* badge = (s.scenarios[i].mode == RunMode::Loop) ? "loop" : "once";
                         uint16_t bg  = isSel ? colors.selBg  : colors.bg;
                         uint16_t fg  = isSel ? colors.selText : Theme::kDim;
                         g2.setTextColor(fg, bg);
                         g2.setTextSize(Theme::kSzSmall);
                         int bw = static_cast<int>(strlen(badge)) * 6;
                         g2.setCursor(w - bw - Theme::kMarginX, y + 4);
                         g2.print(badge);
                         g2.setTextSize(layout.textSize);
                     });
    }

    // Render the running action list.
    template <typename GFX>
    void drawActionList(GFX& d, const State& s, int16_t w, int16_t h)
    {
        const Scenario* sc = s.activeScenario;
        if (!sc) return;

        // Header: scenario name + progress counter + mode indicator
        d.setTextSize(Theme::kSzSmall);
        d.setTextColor(Theme::kTitle, Theme::kBg);
        d.setCursor(3, L::kContentY);
        d.print(sc->name);

        char prog[16];
        snprintf(prog, sizeof(prog), " %d/%d", s.currentActionIdx + 1, static_cast<int>(sc->count));
        d.setTextColor(Theme::kDim, Theme::kBg);
        d.print(prog);

        const char* modeTag = (sc->mode == RunMode::Loop) ? " \x18loop" : " \x18once";
        // \x18 = up-arrow glyph in most LCD fonts; use a plain marker if unavailable
        d.setTextColor(sc->mode == RunMode::Loop ? Theme::kActive : Theme::kHint, Theme::kBg);
        d.setCursor(w - 30, L::kContentY);
        d.print(sc->mode == RunMode::Loop ? "loop" : "once");
        d.setTextColor(Theme::kText, Theme::kBg);

        int sepY = L::kContentY + 10;
        d.drawFastHLine(0, sepY, w, Theme::kSep);

        // Action list
        Ui::ListLayout layout;
        layout.top          = sepY + 2;
        layout.left         = Theme::kListLeft;
        layout.lineHeight   = L::kActionLH;
        layout.textSize     = Theme::kSzSmall;
        layout.scrollPadding = 2;

        Ui::ListColors colors;
        colors.bg      = Theme::kBg;
        colors.text    = Theme::kText;
        colors.selBg   = Theme::kListSelBg;
        colors.selText = Theme::kListSelFg;

        int rows = (h - L::kBotBarH - layout.top - layout.scrollPadding) / layout.lineHeight;
        int topIdx = centeredTopIndex(s.currentActionIdx, static_cast<int>(sc->count), rows);

        Ui::listRows(d, layout, colors, topIdx, static_cast<int>(sc->count),
                     s.currentActionIdx,
                     [&](GFX& g2, int i, int y, bool isSel)
                     {
                         const Action& a = sc->actions[i];
                         uint16_t bg = isSel ? colors.selBg : colors.bg;

                         g2.setCursor(layout.left, y);
                         g2.print(a.label);

                         // For Wait actions show the pre-rolled chosen duration on the right.
                         if (a.kind == ActionKind::Wait && s.chosenWaits != nullptr)
                         {
                             char buf[12];
                             fmtDuration(buf, sizeof(buf), s.chosenWaits[i]);
                             uint16_t dimFg = isSel ? colors.selText : Theme::kDim;
                             g2.setTextColor(dimFg, bg);
                             int bw = static_cast<int>(strlen(buf)) * 6;
                             g2.setCursor(w - bw - Theme::kMarginX, y);
                             g2.print(buf);
                         }
                     });
    }

    // ── Main entry point ─────────────────────────────────────────────────────

    template <typename GFX>
    void drawContent(GFX& d, const State& s, uint32_t nowMs, int16_t w, int16_t h)
    {
        d.fillScreen(Theme::kBg);
        drawTopBar(d, s, w);
        drawBottomHint(d, s, nowMs, w, h);

        if (!s.authenticated)
        {
            drawConnectionContent(d, s, w, h);
        }
        else if (s.running)
        {
            drawActionList(d, s, w, h);
        }
        else
        {
            drawScenarioList(d, s, w, h);
        }
    }

} // namespace BleKeyboardView

#endif
