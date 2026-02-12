#include "IrBruteforce.h"

IrBruteforce::IrBruteforce(M5GFX& screen, uint16_t irPin)
: screen_(screen)
, irSender_(irPin)
, delayMs_(100)
, lastSendMs_(0)
, running_(false)
, address_(0)
, command_(0)
, codesSent_(0)
{
}

void IrBruteforce::setDelayMs(uint32_t delayMs)
{
    delayMs_ = delayMs;
}

void IrBruteforce::start()
{
    irSender_.begin();
    address_ = 0;
    command_ = 0;
    codesSent_ = 0;
    lastSendMs_ = 0;
    running_ = true;
    drawProgress();
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
    if (!running_)
    {
        return false;
    }

    uint32_t now = millis();
    if (now - lastSendMs_ < delayMs_)
    {
        return true;
    }

    sendCurrentCode();
    codesSent_++;
    lastSendMs_ = now;

    drawProgress();

    // Advance to next code
    command_++;
    if (command_ > 0xFF)
    {
        command_ = 0;
        address_++;
        if (address_ > 0xFF)
        {
            // Done -- all codes sent
            running_ = false;
            screen_.fillScreen(TFT_BLACK);
            screen_.setTextSize(2);
            screen_.setTextColor(TFT_GREEN, TFT_BLACK);
            screen_.setCursor(8, 40);
            screen_.print("Done!");
            screen_.setTextColor(TFT_WHITE, TFT_BLACK);
            screen_.setCursor(8, 70);
            screen_.print("Press Select");
            return false;
        }
    }

    return true;
}

uint16_t IrBruteforce::currentAddress() const
{
    return address_;
}

uint16_t IrBruteforce::currentCommand() const
{
    return command_;
}

uint32_t IrBruteforce::totalCodes() const
{
    return kTotalCodes;
}

uint32_t IrBruteforce::codesSent() const
{
    return codesSent_;
}

void IrBruteforce::sendCurrentCode()
{
    // Build the standard NEC 32-bit code:
    // address | ~address | command | ~command
    uint32_t code = (static_cast<uint32_t>(address_) << 24) |
                    (static_cast<uint32_t>(~address_ & 0xFF) << 16) |
                    (static_cast<uint32_t>(command_) << 8) |
                    static_cast<uint32_t>(~command_ & 0xFF);

    irSender_.sendNEC(code, 32);
}

void IrBruteforce::drawProgress()
{
    screen_.fillScreen(TFT_BLACK);
    screen_.setTextSize(2);

    // Title
    screen_.setTextColor(TFT_YELLOW, TFT_BLACK);
    screen_.setCursor(8, 4);
    screen_.print("IR Bruteforce");

    // Current code
    screen_.setTextColor(TFT_WHITE, TFT_BLACK);
    screen_.setCursor(8, 30);
    screen_.printf("Addr: 0x%02X", address_);
    screen_.setCursor(8, 50);
    screen_.printf("Cmd:  0x%02X", command_);

    // Progress
    uint32_t percent = (codesSent_ * 100UL) / kTotalCodes;
    screen_.setCursor(8, 76);
    screen_.printf("%lu / %lu", codesSent_, kTotalCodes);
    screen_.setCursor(8, 96);
    screen_.printf("%lu%%", percent);

    // Hint
    screen_.setTextColor(TFT_DARKGREY, TFT_BLACK);
    screen_.setCursor(8, 120);
    screen_.setTextSize(1);
    screen_.print("Select = stop");
}
