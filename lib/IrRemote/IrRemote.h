#ifndef IR_REMOTE_H
#define IR_REMOTE_H

#include <Arduino.h>
#include <M5GFX.h>
#include <NecSender.h>
#include <ListModel.h>
#include "IrCommand.h"

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

    int              selectedIndex() const;
    const IrCommand& selectedCommand() const;

private:
    int visibleRows() const;

    M5GFX&           screen_;
    NecSender        sender_;
    const IrCommand* commands_;
    ListModel        model_;
};

#endif
