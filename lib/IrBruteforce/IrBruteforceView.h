#ifndef IR_BRUTEFORCE_VIEW_H
#define IR_BRUTEFORCE_VIEW_H

#include <Arduino.h>
#include <M5GFX.h>
#include <Ui.h>
#include <Theme.h>

namespace IrBruteforceView
{
    inline void render(M5GFX& g, uint8_t address, uint8_t command,
                       uint32_t codesSent, uint32_t totalCodes)
    {
        uint32_t percent = totalCodes > 0 ? (codesSent * 100UL) / totalCodes : 0;

        Ui::clear(g);
        Ui::title(g, "IR Bruteforce");

        Ui::pos(g, 30, Theme::kText);  g.printf("Addr: 0x%02X", address);
        Ui::pos(g, 50, Theme::kText);  g.printf("Cmd:  0x%02X", command);
        Ui::pos(g, 76, Theme::kText);  g.printf("%lu / %lu", codesSent, totalCodes);
        Ui::pos(g, 96, Theme::kText);  g.printf("%lu%%", percent);

        Ui::hint(g, "Select = stop");
    }

    inline void renderDone(M5GFX& g)
    {
        Ui::clear(g);
        Ui::pos(g, 40, Theme::kOk);    g.print("Done!");
        Ui::pos(g, 70, Theme::kText);  g.print("Press Select");
    }
}

#endif
