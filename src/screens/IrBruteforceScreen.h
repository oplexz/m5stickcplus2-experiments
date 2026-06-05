#ifndef IR_BRUTEFORCE_SCREEN_H
#define IR_BRUTEFORCE_SCREEN_H

#include "Screen.h"
#include <IrBruteforce.h>

class IrBruteforceScreen : public Screen
{
public:
    explicit IrBruteforceScreen(IrBruteforce& bruteforce) : bruteforce_(bruteforce) {}

    void onEnter() override
    {
        bruteforce_.start();
    }

    ScreenId update(Inputs& in) override
    {
        bruteforce_.tick();

        if (in.select.pressed())
        {
            bruteforce_.stop();
            return ScreenId::IrMenu;
        }
        return ScreenId::None;
    }

private:
    IrBruteforce& bruteforce_;
};

#endif
