#include "IrRemote.h"
#include "IrRemoteView.h"
#include <Theme.h>

IrRemote::IrRemote(M5GFX& screen, uint16_t irPin,
                   const IrCommand* commands, size_t count)
: screen_(screen)
, sender_(irPin)
, commands_(commands)
, model_(count)
{}

void IrRemote::begin()
{
    sender_.begin();
}

void IrRemote::draw()
{
    IrRemoteView::render(screen_, commands_, model_.count(),
                         model_.selectedIndex(), model_.topIndex());
}

bool IrRemote::moveUp(bool wrap)
{
    if (!model_.moveUp(wrap)) return false;
    model_.ensureVisible(visibleRows());
    return true;
}

bool IrRemote::moveDown(bool wrap)
{
    if (!model_.moveDown(wrap)) return false;
    model_.ensureVisible(visibleRows());
    return true;
}

void IrRemote::sendSelected()
{
    if (model_.count() == 0) return;
    const IrCommand& cmd = commands_[model_.selectedIndex()];
    sender_.send(cmd.address, cmd.command);
    IrRemoteView::flashOk(screen_, model_.selectedIndex(), model_.topIndex());
}

int IrRemote::selectedIndex() const
{
    return model_.selectedIndex();
}

const IrCommand& IrRemote::selectedCommand() const
{
    return commands_[model_.selectedIndex()];
}

int IrRemote::visibleRows() const
{
    return (screen_.height() - Theme::kListTop - Theme::kScrollPadding) / Theme::kLineHeight;
}
