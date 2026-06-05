#ifndef SCROLL_LIST_H
#define SCROLL_LIST_H

#include <Arduino.h>
#include <M5GFX.h>
#include <Ui.h>
#include <ListModel.h>

class ScrollList
{
public:
    ScrollList(M5GFX& screen, const char* const* items, size_t count);

    void setLayout(int listTop, int listLeft, int lineHeight, int textSize, int scrollPadding);
    void setColors(uint16_t textColor,
                   uint16_t backgroundColor,
                   uint16_t highlightBackgroundColor,
                   uint16_t highlightTextColor);

    void draw();
    bool moveUp(bool wrap);
    bool moveDown(bool wrap);

    int selectedIndex() const;

private:
    int visibleRows() const;

    M5GFX&             screen_;
    const char* const* items_;
    ListModel          model_;
    Ui::ListLayout     layout_;
    Ui::ListColors     colors_;
};

#endif
