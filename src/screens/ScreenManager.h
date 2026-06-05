#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "Screen.h"

// Routes tick() calls to the currently active Screen, switching screens when
// update() returns a non-None ScreenId. All Screen pointers are externally
// owned; ScreenManager never allocates memory.
class ScreenManager
{
public:
    void registerScreen(ScreenId id, Screen* screen)
    {
        size_t idx = static_cast<size_t>(id);
        if (idx < kMaxScreens)
        {
            screens_[idx] = screen;
        }
    }

    void begin(ScreenId initialId)
    {
        current_ = find(initialId);
        if (current_) current_->onEnter();
    }

    void tick(Inputs& inputs)
    {
        if (!current_) return;
        ScreenId next = current_->update(inputs);
        if (next != ScreenId::None)
        {
            current_ = find(next);
            if (current_) current_->onEnter();
        }
    }

private:
    Screen* find(ScreenId id)
    {
        size_t idx = static_cast<size_t>(id);
        return (idx < kMaxScreens) ? screens_[idx] : nullptr;
    }

    // Sized to cover all ScreenId values (None=0 through BleKeyboard=8).
    static constexpr size_t kMaxScreens = 9;
    Screen* screens_[kMaxScreens] = {};
    Screen* current_              = nullptr;
};

#endif
