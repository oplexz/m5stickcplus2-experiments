#ifndef VALUE_EDITOR_VIEW_H
#define VALUE_EDITOR_VIEW_H

#include <Arduino.h>
#include <M5GFX.h>
#include <Ui.h>
#include <Theme.h>

namespace ValueEditorView
{
    inline void render(M5GFX& g, const char* label, int value, const char* suffix)
    {
        Ui::clear(g);
        Ui::pos(g, Theme::kTitleY + 4, Theme::kText);
        g.print(label);
        g.setTextSize(Theme::kSzLarge);
        g.setCursor(Theme::kMarginX, 48);
        g.printf("%d%s", value, suffix);
    }
}

#endif
