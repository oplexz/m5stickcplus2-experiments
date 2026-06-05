#ifndef BRIGHTNESS_SCREEN_H
#define BRIGHTNESS_SCREEN_H

#include "Screen.h"
#include <M5GFX.h>
#include <ValueEditor.h>
#include "../input/RepeatButton.h"

class BrightnessScreen : public Screen
{
public:
    BrightnessScreen(M5GFX& screen, ValueEditor& editor,
                     Button& upBtn, Button& downBtn,
                     uint32_t repeatDelayMs, uint32_t repeatIntervalMs)
        : screen_(screen)
        , editor_(editor)
        , repUp_(upBtn, repeatDelayMs, repeatIntervalMs)
        , repDown_(downBtn, repeatDelayMs, repeatIntervalMs)
    {}

    void onEnter() override
    {
        repUp_.reset();
        repDown_.reset();
        editor_.draw();
    }

    ScreenId update(Inputs& in) override
    {
        if (repUp_.poll(in.now))
        {
            if (editor_.decrease())
            {
                screen_.setBrightness(percentToBrightness(editor_.value()));
                editor_.draw();
            }
        }
        if (repDown_.poll(in.now))
        {
            if (editor_.increase())
            {
                screen_.setBrightness(percentToBrightness(editor_.value()));
                editor_.draw();
            }
        }
        if (in.select.pressed())
        {
            return ScreenId::List;
        }
        return ScreenId::None;
    }

private:
    static uint8_t percentToBrightness(int percent)
    {
        if (percent < 0)   percent = 0;
        if (percent > 100) percent = 100;
        return static_cast<uint8_t>((percent * 255) / 100);
    }

    M5GFX&       screen_;
    ValueEditor& editor_;
    RepeatButton repUp_;
    RepeatButton repDown_;
};

#endif
