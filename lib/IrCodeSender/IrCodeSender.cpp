#include "IrCodeSender.h"

IrCodeSender::IrCodeSender(M5GFX& screen, uint16_t irPin)
: screen_(screen)
, irSender_(irPin)
, codeIndex_(0)
{
}

void IrCodeSender::begin()
{
    irSender_.begin();
}

void IrCodeSender::draw()
{
    screen_.fillScreen(TFT_BLACK);
    drawCode();
}

bool IrCodeSender::next()
{
    if (codeIndex_ >= kTotalCodes - 1)
    {
        codeIndex_ = 0;
    }
    else
    {
        codeIndex_++;
    }
    drawCode();
    return true;
}

bool IrCodeSender::prev()
{
    if (codeIndex_ == 0)
    {
        codeIndex_ = kTotalCodes - 1;
    }
    else
    {
        codeIndex_--;
    }
    drawCode();
    return true;
}

void IrCodeSender::send()
{
    uint8_t addr = address();
    uint8_t cmd = command();

    uint32_t code = (static_cast<uint32_t>(addr) << 24) |
                    (static_cast<uint32_t>(~addr & 0xFF) << 16) |
                    (static_cast<uint32_t>(cmd) << 8) |
                    static_cast<uint32_t>(~cmd & 0xFF);

    irSender_.sendNEC(code, 32);

    // Flash a brief "SENT" indicator
    screen_.fillRect(0, 100, screen_.width(), 20, TFT_BLACK);
    screen_.setTextSize(2);
    screen_.setTextColor(TFT_GREEN, TFT_BLACK);
    screen_.setCursor(8, 100);
    screen_.print("SENT!");
}

uint8_t IrCodeSender::address() const
{
    return static_cast<uint8_t>(codeIndex_ >> 8);
}

uint8_t IrCodeSender::command() const
{
    return static_cast<uint8_t>(codeIndex_ & 0xFF);
}

uint32_t IrCodeSender::codeIndex() const
{
    return codeIndex_;
}

void IrCodeSender::drawCode()
{
    screen_.fillScreen(TFT_BLACK);
    screen_.setTextSize(2);

    // Title
    screen_.setTextColor(TFT_YELLOW, TFT_BLACK);
    screen_.setCursor(8, 4);
    screen_.print("IR Send");

    // Code index
    screen_.setTextColor(TFT_DARKGREY, TFT_BLACK);
    screen_.setCursor(8, 28);
    screen_.printf("#%lu", codeIndex_);

    // Address + command
    screen_.setTextColor(TFT_WHITE, TFT_BLACK);
    screen_.setTextSize(3);
    screen_.setCursor(8, 52);
    screen_.printf("%02X:%02X", address(), command());

    // Hints
    screen_.setTextSize(1);
    screen_.setTextColor(TFT_DARKGREY, TFT_BLACK);
    screen_.setCursor(8, 120);
    screen_.print("Sel=send  Hold=back");
}
