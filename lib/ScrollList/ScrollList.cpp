#include "ScrollList.h"

ScrollList::ScrollList(M5GFX& screen, const char* const* items, size_t count)
: screen_(screen)
, items_(items)
, model_(count)
{}

void ScrollList::setLayout(int listTop, int listLeft, int lineHeight,
                            int textSize, int scrollPadding)
{
    layout_.top          = listTop;
    layout_.left         = listLeft;
    layout_.lineHeight   = lineHeight;
    layout_.textSize     = textSize;
    layout_.scrollPadding = scrollPadding;
}

void ScrollList::setColors(uint16_t textColor,
                            uint16_t backgroundColor,
                            uint16_t highlightBackgroundColor,
                            uint16_t highlightTextColor)
{
    colors_.text    = textColor;
    colors_.bg      = backgroundColor;
    colors_.selBg   = highlightBackgroundColor;
    colors_.selText = highlightTextColor;
}

void ScrollList::draw()
{
    screen_.fillScreen(colors_.bg);
    Ui::listRows(screen_, layout_, colors_,
                 model_.topIndex(), static_cast<int>(model_.count()),
                 model_.selectedIndex(),
                 [this](M5GFX& g, int i, int y, bool)
                 {
                     g.setCursor(layout_.left, y);
                     g.print(items_[i]);
                 });
}

bool ScrollList::moveUp(bool wrap)
{
    if (!model_.moveUp(wrap)) return false;
    model_.ensureVisible(visibleRows());
    return true;
}

bool ScrollList::moveDown(bool wrap)
{
    if (!model_.moveDown(wrap)) return false;
    model_.ensureVisible(visibleRows());
    return true;
}

int ScrollList::selectedIndex() const
{
    return model_.selectedIndex();
}

int ScrollList::visibleRows() const
{
    return (screen_.height() - layout_.top - layout_.scrollPadding) / layout_.lineHeight;
}
