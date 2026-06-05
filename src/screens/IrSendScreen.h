#ifndef IR_SEND_SCREEN_H
#define IR_SEND_SCREEN_H

#include "Screen.h"
#include <IrCodeSender.h>
#include "../input/RepeatButton.h"
#include "../input/HoldButton.h"

class IrSendScreen : public Screen
{
public:
    IrSendScreen(IrCodeSender& sender,
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

        switch (holdSelect_.poll(in.now))
        {
            case HoldButton::Event::ShortPress:
                sender_.send();
                break;
            case HoldButton::Event::LongPress:
                return ScreenId::IrMenu;
            default:
                break;
        }
        return ScreenId::None;
    }

private:
    IrCodeSender& sender_;
    RepeatButton  repUp_;
    RepeatButton  repDown_;
    HoldButton    holdSelect_;
};

#endif
