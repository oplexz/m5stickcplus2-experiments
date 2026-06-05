#ifndef IR_REPEAT_SCREEN_H
#define IR_REPEAT_SCREEN_H

#include "Screen.h"
#include <IrRepeatSender.h>
#include "../input/RepeatButton.h"
#include "../input/HoldButton.h"

class IrRepeatScreen : public Screen
{
public:
    IrRepeatScreen(IrRepeatSender& sender,
                   Button& upBtn, Button& downBtn, Button& selectBtn,
                   uint32_t repeatDelayMs, uint32_t repeatIntervalMs,
                   uint32_t longPressMs)
        : sender_(sender)
        , repUp_(upBtn, repeatDelayMs, repeatIntervalMs)
        , repDown_(downBtn, repeatDelayMs, repeatIntervalMs)
        , holdSelect_(selectBtn, longPressMs)
    {}

    void onEnter() override
    {
        repUp_.reset();
        repDown_.reset();
        holdSelect_.reset();
        sender_.draw();
    }

    ScreenId update(Inputs& in) override
    {
        if (repUp_.poll(in.now))   sender_.prev();
        if (repDown_.poll(in.now)) sender_.next();

        sender_.tick();

        switch (holdSelect_.poll(in.now))
        {
            case HoldButton::Event::ShortPress:
                if (sender_.isSending()) sender_.stopSending();
                else                     sender_.startSending();
                break;
            case HoldButton::Event::LongPress:
                sender_.stopSending();
                return ScreenId::IrMenu;
            default:
                break;
        }
        return ScreenId::None;
    }

private:
    IrRepeatSender& sender_;
    RepeatButton    repUp_;
    RepeatButton    repDown_;
    HoldButton      holdSelect_;
};

#endif
