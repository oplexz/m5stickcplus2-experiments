#include "ScrollList.h"

ScrollList::ScrollList(M5GFX& screen, const char* const* items, size_t count)
: screen_(screen)
, items_(items)
, count_(count)
, selectedIndex_(0)
, topIndex_(0)
, listTop_(8)
, listLeft_(8)
, lineHeight_(20)
, textSize_(2)
, scrollPadding_(4)
, textColor_(TFT_WHITE)
, backgroundColor_(TFT_BLACK)
, highlightBackgroundColor_(TFT_DARKGREY)
, highlightTextColor_(TFT_BLACK)
{
}

void ScrollList::setLayout(int listTop, int listLeft, int lineHeight, int textSize, int scrollPadding)
{
    listTop_ = listTop;
    listLeft_ = listLeft;
    lineHeight_ = lineHeight;
    textSize_ = textSize;
    scrollPadding_ = scrollPadding;
}

void ScrollList::setColors(uint16_t textColor,
                           uint16_t backgroundColor,
                           uint16_t highlightBackgroundColor,
                           uint16_t highlightTextColor)
{
    textColor_ = textColor;
    backgroundColor_ = backgroundColor;
    highlightBackgroundColor_ = highlightBackgroundColor;
    highlightTextColor_ = highlightTextColor;
}

int ScrollList::visibleRows() const
{
    return (screen_.height() - listTop_ - scrollPadding_) / lineHeight_;
}

void ScrollList::draw()
{
    screen_.fillScreen(backgroundColor_);
    screen_.setTextSize(textSize_);
    screen_.setTextColor(textColor_, backgroundColor_);

    int rows = visibleRows();
    for (int row = 0; row < rows; ++row)
    {
        int itemIndex = topIndex_ + row;
        if (itemIndex >= static_cast<int>(count_))
        {
            break;
        }

        int y = listTop_ + row * lineHeight_;
        bool isSelected = (itemIndex == selectedIndex_);

        if (isSelected)
        {
            screen_.fillRect(0, y - 1, screen_.width(), lineHeight_, highlightBackgroundColor_);
            screen_.setTextColor(highlightTextColor_, highlightBackgroundColor_);
        }
        else
        {
            screen_.setTextColor(textColor_, backgroundColor_);
        }

        screen_.setCursor(listLeft_, y);
        screen_.print(items_[itemIndex]);
    }
}

bool ScrollList::moveUp(bool wrap)
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

bool ScrollList::moveDown(bool wrap)
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

int ScrollList::selectedIndex() const
{
    return selectedIndex_;
}

void ScrollList::ensureSelectionVisible()
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
