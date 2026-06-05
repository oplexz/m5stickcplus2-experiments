#include "IrRepeatSender.h"
#include "IrRepeatSenderView.h"

IrRepeatSender::IrRepeatSender(M5GFX& screen, uint16_t irPin)
: screen_(screen)
, sender_(irPin)
, repeatIntervalMs_(110)
, lastSendMs_(0)
, sending_(false)
, sendCount_(0)
{}

void IrRepeatSender::begin()
{
    sender_.begin();
}

void IrRepeatSender::setRepeatIntervalMs(uint32_t intervalMs)
{
    repeatIntervalMs_ = intervalMs;
}

void IrRepeatSender::draw()
{
    IrRepeatSenderView::render(screen_,
                               codeIndex_.address(), codeIndex_.command(), codeIndex_.value(),
                               sending_, sendCount_);
}

bool IrRepeatSender::next()
{
    codeIndex_.next();
    sendCount_  = 0;
    lastSendMs_ = 0;
    IrRepeatSenderView::render(screen_,
                               codeIndex_.address(), codeIndex_.command(), codeIndex_.value(),
                               sending_, sendCount_);
    return true;
}

bool IrRepeatSender::prev()
{
    codeIndex_.prev();
    sendCount_  = 0;
    lastSendMs_ = 0;
    IrRepeatSenderView::render(screen_,
                               codeIndex_.address(), codeIndex_.command(), codeIndex_.value(),
                               sending_, sendCount_);
    return true;
}

void IrRepeatSender::tick()
{
    if (!sending_) return;

    uint32_t now = millis();
    if (now - lastSendMs_ >= repeatIntervalMs_)
    {
        sender_.send(codeIndex_.address(), codeIndex_.command());
        lastSendMs_ = now;
        sendCount_++;
        IrRepeatSenderView::renderStatus(screen_, sending_, sendCount_);
    }
}

void IrRepeatSender::startSending()
{
    sending_    = true;
    sendCount_  = 0;
    lastSendMs_ = 0;
    IrRepeatSenderView::render(screen_,
                               codeIndex_.address(), codeIndex_.command(), codeIndex_.value(),
                               sending_, sendCount_);
}

void IrRepeatSender::stopSending()
{
    sending_ = false;
    IrRepeatSenderView::render(screen_,
                               codeIndex_.address(), codeIndex_.command(), codeIndex_.value(),
                               sending_, sendCount_);
}

bool IrRepeatSender::isSending() const
{
    return sending_;
}

uint8_t IrRepeatSender::address() const
{
    return codeIndex_.address();
}

uint8_t IrRepeatSender::command() const
{
    return codeIndex_.command();
}

uint32_t IrRepeatSender::codeIndex() const
{
    return codeIndex_.value();
}
