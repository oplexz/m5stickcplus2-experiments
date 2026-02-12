#ifndef IR_REMOTE_H
#define IR_REMOTE_H

#include <Arduino.h>
#include <M5GFX.h>
#include <IRsend.h>

struct IrCommand
{
    const char* name;
    uint8_t address;
    uint8_t command;
};

class IrRemote
{
  public:
    IrRemote(M5GFX& screen, uint16_t irPin,
             const IrCommand* commands, size_t count);

    void begin();
    void draw();

    bool moveUp(bool wrap);
    bool moveDown(bool wrap);
    void sendSelected();

    int selectedIndex() const;
    const IrCommand& selectedCommand() const;

  private:
    void ensureSelectionVisible();
    int visibleRows() const;
    void drawList();

    M5GFX& screen_;
    IRsend irSender_;
    const IrCommand* commands_;
    size_t count_;

    int selectedIndex_;
    int topIndex_;

    static constexpr int kListTop = 8;
    static constexpr int kListLeft = 8;
    static constexpr int kLineHeight = 20;
    static constexpr int kTextSize = 2;
    static constexpr int kScrollPadding = 4;
};

#endif
