#include "IrBruteforce.h"
#include "IrBruteforceView.h"

IrBruteforce::IrBruteforce(M5GFX& screen, uint16_t irPin)
: screen_(screen)
, sender_(irPin)
, delayMs_(100)
, lastSendMs_(0)
, running_(false)
, address_(0)
, command_(0)
, codesSent_(0)
{}

void IrBruteforce::setDelayMs(uint32_t delayMs)
{
    delayMs_ = delayMs;
}

void IrBruteforce::start()
{
    sender_.begin();
    address_    = 0;
    command_    = 0;
    codesSent_  = 0;
    lastSendMs_ = 0;
    running_    = true;
    IrBruteforceView::render(screen_,
                             static_cast<uint8_t>(address_), static_cast<uint8_t>(command_),
                             codesSent_, kTotalCodes);
}

void IrBruteforce::stop()
{
    running_ = false;
}

bool IrBruteforce::isRunning() const
{
    return running_;
}

bool IrBruteforce::tick()
{
    if (!running_) return false;

    uint32_t now = millis();
    if (now - lastSendMs_ < delayMs_) return true;

    sender_.send(static_cast<uint8_t>(address_), static_cast<uint8_t>(command_));
    codesSent_++;
    lastSendMs_ = now;

    IrBruteforceView::render(screen_,
                             static_cast<uint8_t>(address_), static_cast<uint8_t>(command_),
                             codesSent_, kTotalCodes);

    command_++;
    if (command_ > 0xFF)
    {
        command_ = 0;
        address_++;
        if (address_ > 0xFF)
        {
            running_ = false;
            IrBruteforceView::renderDone(screen_);
            return false;
        }
    }

    return true;
}

uint16_t IrBruteforce::currentAddress() const { return address_; }
uint16_t IrBruteforce::currentCommand() const { return command_; }
uint32_t IrBruteforce::totalCodes()     const { return kTotalCodes; }
uint32_t IrBruteforce::codesSent()      const { return codesSent_; }
