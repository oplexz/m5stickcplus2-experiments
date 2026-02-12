#include "IrRepeatSender.h"

IrRepeatSender::IrRepeatSender(M5GFX& screen, uint16_t irPin)
: screen_(screen)
, irSender_(irPin)
, codeIndex_(0)
, repeatIntervalMs_(110)
, lastSendMs_(0)
, sending_(false)
, sendCount_(0)
{
}

void IrRepeatSender::begin()
{
    irSender_.begin();
}

void IrRepeatSender::setRepeatIntervalMs(uint32_t intervalMs)
{
    repeatIntervalMs_ = intervalMs;
}

void IrRepeatSender::draw()
{
    drawScreen();
}

bool IrRepeatSender::next()
{
    if (codeIndex_ >= kTotalCodes - 1)
    {
        codeIndex_ = 0;
    }
    else
    {
        codeIndex_++;
    }
    sendCount_ = 0;
    lastSendMs_ = 0; // send immediately on next tick
    drawScreen();
    return true;
}

bool IrRepeatSender::prev()
{
    if (codeIndex_ == 0)
    {
        codeIndex_ = kTotalCodes - 1;
    }
    else
    {
        codeIndex_--;
    }
    sendCount_ = 0;
    lastSendMs_ = 0;
    drawScreen();
    return true;
}

void IrRepeatSender::tick()
{
    if (!sending_)
    {
        return;
    }

    uint32_t now = millis();
    if (now - lastSendMs_ >= repeatIntervalMs_)
    {
        sendCode();
        lastSendMs_ = now;
        sendCount_++;
        drawStatus();
    }
}

void IrRepeatSender::startSending()
{
    sending_ = true;
    sendCount_ = 0;
    lastSendMs_ = 0; // fire immediately on next tick
    drawScreen();
}

void IrRepeatSender::stopSending()
{
    sending_ = false;
    drawScreen();
}

bool IrRepeatSender::isSending() const
{
    return sending_;
}

uint8_t IrRepeatSender::address() const
{
    return static_cast<uint8_t>(codeIndex_ >> 8);
}

uint8_t IrRepeatSender::command() const
{
    return static_cast<uint8_t>(codeIndex_ & 0xFF);
}

uint32_t IrRepeatSender::codeIndex() const
{
    return codeIndex_;
}

void IrRepeatSender::sendCode()
{
    uint8_t addr = address();
    uint8_t cmd = command();

    uint32_t code = (static_cast<uint32_t>(addr) << 24) |
                    (static_cast<uint32_t>(~addr & 0xFF) << 16) |
                    (static_cast<uint32_t>(cmd) << 8) |
                    static_cast<uint32_t>(~cmd & 0xFF);

    irSender_.sendNEC(code, 32);
}

void IrRepeatSender::drawScreen()
{
    screen_.fillScreen(TFT_BLACK);
    screen_.setTextSize(2);

    // Title
    screen_.setTextColor(TFT_YELLOW, TFT_BLACK);
    screen_.setCursor(8, 4);
    screen_.print("IR Repeat");

    // Code index
    screen_.setTextColor(TFT_DARKGREY, TFT_BLACK);
    screen_.setCursor(8, 28);
    screen_.printf("#%lu", codeIndex_);

    // Address + command (big)
    screen_.setTextColor(TFT_WHITE, TFT_BLACK);
    screen_.setTextSize(3);
    screen_.setCursor(8, 48);
    screen_.printf("%02X:%02X", address(), command());

    drawStatus();

    // Hints
    screen_.setTextSize(1);
    screen_.setTextColor(TFT_DARKGREY, TFT_BLACK);
    screen_.setCursor(8, 120);
    screen_.print("Sel=start/stop Hold=back");
}

void IrRepeatSender::drawStatus()
{
    // Clear status area
    screen_.fillRect(0, 80, screen_.width(), 30, TFT_BLACK);
    screen_.setTextSize(2);

    if (sending_)
    {
        screen_.setTextColor(TFT_GREEN, TFT_BLACK);
        screen_.setCursor(8, 84);
        screen_.printf("SENDING x%lu", sendCount_);
    }
    else
    {
        screen_.setTextColor(TFT_DARKGREY, TFT_BLACK);
        screen_.setCursor(8, 84);
        screen_.print("Stopped");
    }
}
