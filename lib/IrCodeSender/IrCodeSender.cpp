#include "IrCodeSender.h"
#include "IrCodeSenderView.h"

IrCodeSender::IrCodeSender(M5GFX& screen, uint16_t irPin)
: screen_(screen)
, sender_(irPin)
{}

void IrCodeSender::begin()
{
    sender_.begin();
}

void IrCodeSender::draw()
{
    IrCodeSenderView::render(screen_, codeIndex_.address(), codeIndex_.command(), codeIndex_.value());
}

bool IrCodeSender::next()
{
    codeIndex_.next();
    IrCodeSenderView::render(screen_, codeIndex_.address(), codeIndex_.command(), codeIndex_.value());
    return true;
}

bool IrCodeSender::prev()
{
    codeIndex_.prev();
    IrCodeSenderView::render(screen_, codeIndex_.address(), codeIndex_.command(), codeIndex_.value());
    return true;
}

void IrCodeSender::send()
{
    sender_.send(codeIndex_.address(), codeIndex_.command());
    IrCodeSenderView::renderSent(screen_);
}

uint8_t IrCodeSender::address() const
{
    return codeIndex_.address();
}

uint8_t IrCodeSender::command() const
{
    return codeIndex_.command();
}

uint32_t IrCodeSender::codeIndex() const
{
    return codeIndex_.value();
}
