#ifndef IR_REMOTE_VIEW_H
#define IR_REMOTE_VIEW_H

#include <Arduino.h>
#include <M5GFX.h>
#include <Ui.h>
#include <Theme.h>
#include "IrCommand.h"

namespace IrRemoteView
{
    inline void render(M5GFX& g, const IrCommand* commands, size_t count,
                       int selectedIndex, int topIndex)
    {
        Ui::ListLayout layout;
        Ui::ListColors colors;

        g.fillScreen(Theme::kBg);
        Ui::listRows(g, layout, colors, topIndex, static_cast<int>(count), selectedIndex,
                     [&](M5GFX& g2, int i, int y, bool isSel)
                     {
                         g2.setCursor(layout.left, y);
                         g2.print(commands[i].name);

                         // Dim hex code on the right edge
                         uint16_t bg = isSel ? colors.selBg : colors.bg;
                         g2.setTextColor(Theme::kDim, bg);
                         g2.setTextSize(Theme::kSzSmall);
                         g2.setCursor(g2.width() - 28, y + 4);
                         g2.printf("x%02X", commands[i].command);
                         g2.setTextSize(layout.textSize); // restore for next row
                     });
    }

    // Flash a brief "OK" label next to the selected row after a successful send.
    inline void flashOk(M5GFX& g, int selectedIndex, int topIndex)
    {
        int row  = selectedIndex - topIndex;
        int y    = Theme::kListTop + row * Theme::kLineHeight;
        int sentX = g.width() - 52;
        g.setTextSize(Theme::kSzNormal);
        g.setTextColor(Theme::kOk, Theme::kBg);
        g.setCursor(sentX, y);
        g.print("OK");
    }
}

#endif
