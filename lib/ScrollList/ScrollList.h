#ifndef SCROLL_LIST_H
#define SCROLL_LIST_H

#include <Arduino.h>
#include <M5GFX.h>

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
    void ensureSelectionVisible();

    M5GFX& screen_;
    const char* const* items_;
    size_t count_;

    int selectedIndex_;
    int topIndex_;

    int listTop_;
    int listLeft_;
    int lineHeight_;
    int textSize_;
    int scrollPadding_;

    uint16_t textColor_;
    uint16_t backgroundColor_;
    uint16_t highlightBackgroundColor_;
    uint16_t highlightTextColor_;
};

#endif
