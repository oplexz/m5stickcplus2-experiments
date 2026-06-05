#ifndef REPEAT_BUTTON_H
#define REPEAT_BUTTON_H

#include <Arduino.h>
#include <Button.h>

// Wraps a Button and produces a "tick" on initial press and on each auto-repeat
// interval. Encapsulates the press-start / last-repeat timing used for up/down
// navigation with hold-to-scroll behaviour.
class RepeatButton
{
public:
    RepeatButton(Button& btn, uint32_t repeatDelayMs, uint32_t repeatIntervalMs)
        : btn_(btn)
        , repeatDelayMs_(repeatDelayMs)
        , repeatIntervalMs_(repeatIntervalMs)
        , pressStartMs_(0)
        , lastRepeatMs_(0)
    {}

    void reset()
    {
        pressStartMs_ = 0;
        lastRepeatMs_ = 0;
    }

    // Returns true on the initial press and on every repeat interval thereafter.
    bool poll(uint32_t now)
    {
        bool isPressed = (btn_.read() == Button::PRESSED);
        bool changed   = btn_.has_changed();

        if (!isPressed)
        {
            pressStartMs_ = 0;
            lastRepeatMs_ = 0;
            return false;
        }
        if (changed)
        {
            pressStartMs_ = now;
            lastRepeatMs_ = now;
            return true;
        }
        if (pressStartMs_ != 0 &&
            now - pressStartMs_ >= repeatDelayMs_ &&
            now - lastRepeatMs_ >= repeatIntervalMs_)
        {
            lastRepeatMs_ = now;
            return true;
        }
        return false;
    }

private:
    Button&  btn_;
    uint32_t repeatDelayMs_;
    uint32_t repeatIntervalMs_;
    uint32_t pressStartMs_;
    uint32_t lastRepeatMs_;
};

#endif
