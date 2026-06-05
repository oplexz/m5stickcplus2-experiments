#ifndef IR_CODE_SENDER_VIEW_H
#define IR_CODE_SENDER_VIEW_H

#include <Arduino.h>
#include <M5GFX.h>
#include <Ui.h>
#include <Theme.h>

namespace IrCodeSenderView
{
    inline void render(M5GFX& g, uint8_t addr, uint8_t cmd, uint32_t codeIdx)
    {
        Ui::clear(g);
        Ui::title(g, "IR Send");
        Ui::pos(g, 28, Theme::kDim);  g.printf("#%lu", codeIdx);
        Ui::bigCode(g, 52, addr, cmd);
        Ui::hint(g, "Sel=send  Hold=back");
    }

    inline void renderSent(M5GFX& g)
    {
        g.fillRect(0, 100, g.width(), 20, Theme::kBg);
        Ui::pos(g, 100, Theme::kOk);
        g.print("SENT!");
    }
}

#endif
