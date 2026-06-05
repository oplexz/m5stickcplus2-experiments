#ifndef LIST_MODEL_H
#define LIST_MODEL_H

#include <Arduino.h>

// Pure selection + scroll-window model — no GFX dependency.
// Extracted from the near-identical logic in ScrollList and IrRemote.
//
// Usage pattern:
//   bool changed = model.moveUp(wrap);
//   if (changed) {
//       model.ensureVisible(visibleRowCount);
//       redraw();
//   }
class ListModel
{
public:
    explicit ListModel(size_t count)
    : count_(count), selectedIndex_(0), topIndex_(0)
    {}

    // Move selection, returning true if it actually changed.
    bool moveUp(bool wrap)
    {
        if (count_ == 0) return false;
        if (selectedIndex_ == 0)
        {
            if (!wrap) return false;
            selectedIndex_ = static_cast<int>(count_) - 1;
        }
        else
        {
            selectedIndex_--;
        }
        return true;
    }

    bool moveDown(bool wrap)
    {
        if (count_ == 0) return false;
        if (selectedIndex_ >= static_cast<int>(count_) - 1)
        {
            if (!wrap) return false;
            selectedIndex_ = 0;
        }
        else
        {
            selectedIndex_++;
        }
        return true;
    }

    // Adjust topIndex so the selection stays within the visible window.
    // Call after moveUp / moveDown with the current visible row count.
    void ensureVisible(int rows)
    {
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
        if (topIndex_ < 0) topIndex_ = 0;
    }

    int    selectedIndex() const { return selectedIndex_; }
    int    topIndex()      const { return topIndex_; }
    size_t count()         const { return count_; }

private:
    size_t count_;
    int    selectedIndex_;
    int    topIndex_;
};

#endif
