#ifndef IR_REPEAT_SENDER_VIEW_H
#define IR_REPEAT_SENDER_VIEW_H

#include <Arduino.h>
#include <M5GFX.h>
#include <Ui.h>
#include <Theme.h>

namespace IrRepeatSenderView
{
    // Partial redraw: status area only (called on every tick when sending).
    inline void renderStatus(M5GFX& g, bool sending, uint32_t sendCount)
    {
        g.fillRect(0, 80, g.width(), 30, Theme::kBg);
        Ui::pos(g, 84, sending ? Theme::kOk : Theme::kDim);
        if (sending)
        {
            g.printf("SENDING x%lu", sendCount);
        }
        else
        {
            g.print("Stopped");
        }
    }

    inline void render(M5GFX& g, uint8_t addr, uint8_t cmd, uint32_t codeIdx,
                       bool sending, uint32_t sendCount)
    {
        Ui::clear(g);
        Ui::title(g, "IR Repeat");
        Ui::pos(g, 28, Theme::kDim);  g.printf("#%lu", codeIdx);
        Ui::bigCode(g, 48, addr, cmd);
        renderStatus(g, sending, sendCount);
        Ui::hint(g, "Sel=start/stop Hold=back");
    }
}

#endif
