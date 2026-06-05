#ifndef LIST_SCREEN_H
#define LIST_SCREEN_H

#include "Screen.h"
#include <ScrollList.h>

class ListScreen : public Screen
{
public:
    static constexpr int kBrightnessIndex  = 0;
    static constexpr int kIrToolsIndex     = 1;
    static constexpr int kBleKeyboardIndex = 2;

    explicit ListScreen(ScrollList& list) : list_(list) {}

    void onEnter() override
    {
        list_.draw();
    }

    ScreenId update(Inputs& in) override
    {
        bool updated = false;

        if (in.up.pressed())   updated = list_.moveUp(true);
        if (in.down.pressed()) updated = list_.moveDown(true) || updated;

        if (in.select.pressed())
        {
            switch (list_.selectedIndex())
            {
                case kBrightnessIndex:  return ScreenId::Brightness;
                case kIrToolsIndex:     return ScreenId::IrMenu;
                case kBleKeyboardIndex: return ScreenId::BleKeyboard;
            }
        }

        if (updated) list_.draw();
        return ScreenId::None;
    }

private:
    ScrollList& list_;
};

#endif
