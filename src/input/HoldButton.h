#ifndef HOLD_BUTTON_H
#define HOLD_BUTTON_H

#include <Arduino.h>
#include <Button.h>

// Wraps a Button and distinguishes short-press (released before threshold) from
// long-press (still held once threshold is reached). Matches the select-button
// timing pattern used across several screens.
class HoldButton
{
public:
    enum class Event { None, ShortPress, LongPress };

    HoldButton(Button& btn, uint32_t longPressMs)
        : btn_(btn)
        , longPressMs_(longPressMs)
        , pressStartMs_(0)
    {}

    void reset() { pressStartMs_ = 0; }

    // Call every loop iteration. Returns the event that occurred this frame,
    // or Event::None if nothing changed.
    Event poll(uint32_t now)
    {
        bool isPressed = (btn_.read() == Button::PRESSED);
        bool changed   = btn_.has_changed();

        if (isPressed && changed)
        {
            pressStartMs_ = now;
            return Event::None;
        }
        if (!isPressed && changed && pressStartMs_ != 0)
        {
            uint32_t duration = now - pressStartMs_;
            pressStartMs_ = 0;
            return (duration < longPressMs_) ? Event::ShortPress : Event::None;
        }
        if (isPressed && pressStartMs_ != 0 && (now - pressStartMs_) >= longPressMs_)
        {
            pressStartMs_ = 0;
            return Event::LongPress;
        }
        return Event::None;
    }

private:
    Button&  btn_;
    uint32_t longPressMs_;
    uint32_t pressStartMs_;
};

#endif
