#ifndef UI_H
#define UI_H

#include <Arduino.h>
#include <M5GFX.h>
#include <Theme.h>

// Stateless draw helpers. Every function operates on a caller-supplied M5GFX
// reference so nothing here owns screen state. Views call these instead of
// touching M5GFX directly for common patterns.
namespace Ui
{
    // ── Layout descriptors ────────────────────────────────────────────────────

    struct ListLayout
    {
        int top          = Theme::kListTop;
        int left         = Theme::kListLeft;
        int lineHeight   = Theme::kLineHeight;
        int textSize     = Theme::kSzNormal;
        int scrollPadding = Theme::kScrollPadding;
    };

    struct ListColors
    {
        uint16_t bg      = Theme::kBg;
        uint16_t text    = Theme::kText;
        uint16_t selBg   = Theme::kListSelBg;
        uint16_t selText = Theme::kListSelFg;
    };

    // ── Primitive helpers ─────────────────────────────────────────────────────

    // Fill the screen with the background colour.
    inline void clear(M5GFX& g)
    {
        g.fillScreen(Theme::kBg);
    }

    // Draw a title string at the standard title position.
    inline void title(M5GFX& g, const char* text)
    {
        g.setTextSize(Theme::kSzNormal);
        g.setTextColor(Theme::kTitle, Theme::kBg);
        g.setCursor(Theme::kMarginX, Theme::kTitleY);
        g.print(text);
    }

    // Draw a hint string at the standard footer position.
    inline void hint(M5GFX& g, const char* text)
    {
        g.setTextSize(Theme::kSzSmall);
        g.setTextColor(Theme::kHint, Theme::kBg);
        g.setCursor(Theme::kMarginX, Theme::kHintY);
        g.print(text);
    }

    // Full-width horizontal separator line.
    inline void hline(M5GFX& g, int y, uint16_t color = Theme::kSep)
    {
        g.drawFastHLine(0, y, g.width(), color);
    }

    // Position the cursor at (kMarginX, y) and set colour + text size.
    // The caller then calls g.print / g.printf to emit content.
    inline void pos(M5GFX& g, int y, uint16_t color, int size = Theme::kSzNormal)
    {
        g.setTextColor(color, Theme::kBg);
        g.setTextSize(size);
        g.setCursor(Theme::kMarginX, y);
    }

    // Draw an address:command pair in large text (used by IR screens).
    inline void bigCode(M5GFX& g, int y, uint8_t addr, uint8_t cmd)
    {
        g.setTextColor(Theme::kText, Theme::kBg);
        g.setTextSize(Theme::kSzLarge);
        g.setCursor(Theme::kMarginX, y);
        g.printf("%02X:%02X", addr, cmd);
    }

    // ── List row renderer ─────────────────────────────────────────────────────
    //
    // Iterates the visible window [topIndex, topIndex+rows) and for each item:
    //   1. Fills a highlight rectangle if selected.
    //   2. Sets text colour from colors.
    //   3. Calls drawRow(g, itemIndex, y, isSelected).
    //
    // GFX is templated so this works with both M5GFX (direct screen) and
    // M5Canvas (sprite buffer).  drawRow lambdas should use `auto& g` rather
    // than `M5GFX& g` when they may be instantiated with M5Canvas.
    //
    // drawRow is responsible for printing the row content starting at y.
    // It may change text size / colour for secondary columns but should
    // restore layout.textSize before returning so the next row is consistent.
    //
    template <typename GFX, typename DrawRowFn>
    void listRows(GFX& g, const ListLayout& layout, const ListColors& colors,
                  int topIndex, int count, int selectedIndex,
                  DrawRowFn drawRow)
    {
        int rows = (g.height() - layout.top - layout.scrollPadding) / layout.lineHeight;
        g.setTextSize(layout.textSize);

        for (int row = 0; row < rows; ++row)
        {
            int itemIdx = topIndex + row;
            if (itemIdx >= count) break;

            int  y     = layout.top + row * layout.lineHeight;
            bool isSel = (itemIdx == selectedIndex);

            if (isSel)
            {
                g.fillRect(0, y - 1, g.width(), layout.lineHeight, colors.selBg);
                g.setTextColor(colors.selText, colors.selBg);
            }
            else
            {
                g.setTextColor(colors.text, colors.bg);
            }

            drawRow(g, itemIdx, y, isSel);
        }
    }

} // namespace Ui

#endif
