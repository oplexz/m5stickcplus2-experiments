#include "IrRemote.h"

IrRemote::IrRemote(M5GFX& screen, uint16_t irPin,
                   const IrCommand* commands, size_t count)
: screen_(screen)
, irSender_(irPin)
, commands_(commands)
, count_(count)
, selectedIndex_(0)
, topIndex_(0)
{
}

void IrRemote::begin()
{
    irSender_.begin();
}

void IrRemote::draw()
{
    drawList();
}

bool IrRemote::moveUp(bool wrap)
{
    if (count_ == 0)
    {
        return false;
    }

    if (selectedIndex_ == 0)
    {
        if (!wrap)
        {
            return false;
        }
        selectedIndex_ = static_cast<int>(count_) - 1;
    }
    else
    {
        selectedIndex_--;
    }

    ensureSelectionVisible();
    return true;
}

bool IrRemote::moveDown(bool wrap)
{
    if (count_ == 0)
    {
        return false;
    }

    if (selectedIndex_ >= static_cast<int>(count_) - 1)
    {
        if (!wrap)
        {
            return false;
        }
        selectedIndex_ = 0;
    }
    else
    {
        selectedIndex_++;
    }

    ensureSelectionVisible();
    return true;
}

void IrRemote::sendSelected()
{
    if (count_ == 0)
    {
        return;
    }

    const IrCommand& cmd = commands_[selectedIndex_];

    uint32_t code = (static_cast<uint32_t>(cmd.address) << 24) |
                    (static_cast<uint32_t>(~cmd.address & 0xFF) << 16) |
                    (static_cast<uint32_t>(cmd.command) << 8) |
                    static_cast<uint32_t>(~cmd.command & 0xFF);

    irSender_.sendNEC(code, 32);

    // Flash "SENT" next to selected item
    int row = selectedIndex_ - topIndex_;
    int y = kListTop + row * kLineHeight;
    int sentX = screen_.width() - 52;
    screen_.setTextSize(2);
    screen_.setTextColor(TFT_GREEN, TFT_BLACK);
    screen_.setCursor(sentX, y);
    screen_.print("OK");
}

int IrRemote::selectedIndex() const
{
    return selectedIndex_;
}

const IrCommand& IrRemote::selectedCommand() const
{
    return commands_[selectedIndex_];
}

int IrRemote::visibleRows() const
{
    return (screen_.height() - kListTop - kScrollPadding) / kLineHeight;
}

void IrRemote::ensureSelectionVisible()
{
    int rows = visibleRows();
    if (rows <= 0)
    {
        topIndex_ = 0;
        return;
    }

    if (selectedIndex_ < topIndex_)
    {
        topIndex_ = selectedIndex_;
    }
    else if (selectedIndex_ >= topIndex_ + rows)
    {
        topIndex_ = selectedIndex_ - rows + 1;
    }

    if (topIndex_ < 0)
    {
        topIndex_ = 0;
    }
}

void IrRemote::drawList()
{
    screen_.fillScreen(TFT_BLACK);
    screen_.setTextSize(kTextSize);

    int rows = visibleRows();
    for (int row = 0; row < rows; ++row)
    {
        int itemIndex = topIndex_ + row;
        if (itemIndex >= static_cast<int>(count_))
        {
            break;
        }

        int y = kListTop + row * kLineHeight;
        bool isSelected = (itemIndex == selectedIndex_);
        const IrCommand& cmd = commands_[itemIndex];

        if (isSelected)
        {
            screen_.fillRect(0, y - 1, screen_.width(), kLineHeight, TFT_DARKGREY);
            screen_.setTextColor(TFT_BLACK, TFT_DARKGREY);
        }
        else
        {
            screen_.setTextColor(TFT_WHITE, TFT_BLACK);
        }

        screen_.setCursor(kListLeft, y);
        screen_.print(cmd.name);

        // Show hex code on the right
        uint16_t bg = isSelected ? TFT_DARKGREY : TFT_BLACK;
        uint16_t fg = isSelected ? TFT_BLACK : TFT_DARKGREY;
        screen_.setTextColor(fg, bg);
        screen_.setTextSize(1);
        int hexX = screen_.width() - 28;
        screen_.setCursor(hexX, y + 4);
        screen_.printf("x%02X", cmd.command);
        screen_.setTextSize(kTextSize);
    }
}
